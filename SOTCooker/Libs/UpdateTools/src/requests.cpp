#include <QJsonArray>
#include <QJsonObject>
#include <iostream>

#include <Logger/logger.hpp>
#include <UpdateTools/network.hpp>
#include <UpdateTools/requests.hpp>

namespace updt {

std::ostream& operator<<(std::ostream& os, const DistantVersion& dv) {
  QString assets_str{};
  for (const QString& asset : dv.assets) {
    assets_str.append(asset + ", ");
  }
  if (!assets_str.isEmpty()) {
    assets_str.chop(2);
  }
  return os << "DistantVersion{.version=" << gitversion::to_string(dv.version) << ", .is_preview: " << dv.is_preview
            << ", .assets: [" << assets_str.toStdString() << "], .installer_url: " << dv.installer_url.toStdString()
            << " }";
}

void RetrieveLatestDistantVersion(const QString& github_releases_url,
                                  std::function<void(std::optional<DistantVersion>)> callback, bool search_preview) {
  GetJsonFromAPI(github_releases_url, [=](std::optional<QJsonDocument> json_opt) {
    if (!json_opt.has_value()) {
      SPDLOG_ERROR("Cannot retrieve latest version from github");
      callback(std::nullopt);
      return;
    }

    const auto kVersions{ParseDistantVersion(json_opt)};
    if (kVersions.empty()) {
      SPDLOG_ERROR("Cannot parse distant versions");
      callback(std::nullopt);
      return;
    }

    const auto kFoundIter{std::find_if(
        kVersions.rbegin(), kVersions.rend(), [&](const std::pair<gitversion::Version, DistantVersion>& input) {
          const auto& [git_version, version]{input};
          if (search_preview || !version.is_preview) {
            callback(version);
            return true;
          }
          return false;
        })};

    if (kFoundIter != kVersions.rend()) {
      return;
    }

    SPDLOG_WARN("No latest version found... Kinda weird.");
    callback(std::nullopt);
  });
}

const QString& FindInstallerUrl(const QStringList& assets) {
  static QString kDefault{};
  if (assets.isEmpty()) {
    return kDefault;
  }
  for (const auto& asset : assets) {
    if (asset.endsWith("_setup_x64.exe")) {
      return asset;
    }
  }
  return kDefault;
}

std::map<gitversion::Version, DistantVersion> ParseDistantVersion(std::optional<QJsonDocument> json_opt) {
  if (!json_opt.has_value()) {
    return {};
  }
  SPDLOG_DEBUG("------------------------------------------------------------ UPDT");

  const auto kJson{json_opt.value().array()};
  const auto kVersionJson{kJson.at(0).toArray()};

  std::map<gitversion::Version, DistantVersion> out{};
  for (const QJsonValue& value : kVersionJson.at(0).toArray()) {
    if (!value.isObject()) {
      SPDLOG_ERROR("Cannot parse json value: {}", value.type());
      continue;
    }
    QJsonObject obj{value.toObject()};
    DistantVersion version{};

    version.version = gitversion::FromTag(obj["tag_name"].toString().toStdString());
    version.is_preview = obj["prerelease"].toBool();

    const QJsonArray assets{obj["assets"].toArray()};

    for (const QJsonValue& asset : assets.at(0).toArray()) {
      version.assets.emplace_back(asset.toObject()["browser_download_url"].toString());
    }

    version.installer_url = FindInstallerUrl(version.assets);

    out[version.version] = version;
  }
  return out;
}
}  // namespace updt
