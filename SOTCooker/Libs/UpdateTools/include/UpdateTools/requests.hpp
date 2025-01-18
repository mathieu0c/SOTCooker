#pragma once

#include <QStringList>
#include <git_version_structs.hpp>
#include <qjsondocument.h>

#include <Logger/logger.hpp>

namespace updt {

struct DistantVersion {
  gitversion::Version version{};
  bool is_preview{};

  QStringList assets{};
  QString installer_url{};
};

inline bool operator==(const DistantVersion& r, const DistantVersion& d) {
  return r.version == d.version && r.is_preview == d.is_preview;
}

inline bool operator>(const DistantVersion& r, const DistantVersion& d) {
  if (r.version > d.version) return true;
  if (r.version < d.version) return false;
  return r.is_preview && !d.is_preview;
}

std::ostream& operator<<(std::ostream& os, const DistantVersion& dv);

void RetrieveLatestDistantVersion(const QString& github_releases_url,
                                  std::function<void(std::optional<DistantVersion>)> callback, bool search_preview);

const QString& FindInstallerUrl(const QStringList& assets);
std::map<gitversion::Version, DistantVersion> ParseDistantVersion(std::optional<QJsonDocument> json_opt);

}  // namespace updt
