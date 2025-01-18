#include "PathUtils.hpp"

#include <QDesktopServices>
#include <QMessageBox>
#include <QStandardPaths>

namespace sot {

QString GetAppDataPath() {
  return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

void RetrieveUpdate(bool display_info_if_no_update_advised) {
  updt::RetrieveLatestDistantVersion(
      updt::GithubReleasesUrl("mathieu0c", "SOTCooker"),
      [=](std::optional<updt::DistantVersion> version) {
        sot::OnUpdateRetrieved(version, display_info_if_no_update_advised);
      },
      false);
}

void OnUpdateRetrieved(std::optional<updt::DistantVersion> version, bool display_info_if_no_update_advised) {
  if (!version) {
    SPDLOG_ERROR("Failed to retrieve update version");
    if (display_info_if_no_update_advised) {
      QMessageBox::information(
          nullptr, QObject::tr("Update check"), QObject::tr("Failed to retrieve distant versions availables"));
    }
    return;
  }

  SPDLOG_INFO("== Update / version info retrieved ==");
  SPDLOG_INFO("\tCurrent version: <{}>", gitversion::GetVersionRepresentationString());
  SPDLOG_INFO("\tLatest version: <{}>", version->version);
  SPDLOG_INFO("\tDownload URL: <{}>\n", version->installer_url);

  if (!(version->version > gitversion::GetInfo().last_known_version || /* Conditions for a new udpate available */
        (version->version == gitversion::GetInfo().last_known_version && !version->is_preview))) {
    SPDLOG_INFO("No new version available: {} <= {}", *version, gitversion::GetInfo().last_known_version);

    if (display_info_if_no_update_advised) {
      QMessageBox::information(nullptr,
                               QObject::tr("Update check"),
                               QObject::tr("No new version available.\n  Running: %0\n  Latest: %1")
                                   .arg(QString::fromStdString(gitversion::GetVersionRepresentationString()))
                                   .arg(QString::fromStdString(gitversion::to_string(version->version))));
    }

    return;
  }

  if (version->installer_url.isEmpty()) {
    SPDLOG_ERROR("No installer URL available");
    return;
  }

  SPDLOG_INFO("New version available: {} > {}", *version, gitversion::GetInfo().last_known_version);
  const auto kQuestionText{
      QObject::tr("New version available.\n  Running: %0\n  Latest available: %1\n\nDo you want to update?")
          .arg(QString::fromStdString(gitversion::GetVersionRepresentationString()))
          .arg(QString::fromStdString(gitversion::to_string(version->version)))};
  [[maybe_unused]] const auto kReply{QMessageBox::question(nullptr, QObject::tr("Update available"), kQuestionText)};

  if (kReply == QMessageBox::Yes) {
    QDesktopServices::openUrl(QUrl{version->installer_url});
  }
}

}  // namespace sot
