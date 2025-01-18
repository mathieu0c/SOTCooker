#include <PathUtils.hpp>
#include <QApplication>
#include <QFontDatabase>
#include <QLocale>
#include <QMessageBox>
#include <QTranslator>
#include <TextToSpeech.hpp>
#include <git_version.hpp>

#include <Logger/btype.hpp>
#include <Logger/logger_setup.hpp>
#include <UpdateTools/network.hpp>
#include <UpdateTools/requests.hpp>

#include "LoggerHandler.hpp"
#include "MainWindow.hpp"

bool SetupFolders() {
  auto lambda_create_folder_if_not_exists = [](const QString &path) {
    QDir dir{path};
    if (dir.exists()) {
      return true;
    }
    const auto kSuccess{dir.mkpath(".")};
    if (!kSuccess) {
      SPDLOG_ERROR("Failed to create folder: <{}>", path);
    }
    return kSuccess;
  };

  bool success{true};
  success &= lambda_create_folder_if_not_exists(sot::GetAppDataPath());
  return success;
}

int main(int argc, char *argv[]) {
  SPDLOG_INFO("DEBUG");

  QApplication a(argc, argv);

  /* -- Setting up logger -- */
  const auto kLogPath{logger::GetLogFilePath(CMAKEMACRO_PROJECT_EXE)};
  logger::SetupLoggerRotating(kLogPath, 2);
  if (!SetupFolders()) {
    SPDLOG_ERROR("Failed to setup folders");
    QApplication err_app(argc, argv);
    QMessageBox::critical(nullptr,
                          QObject::tr("Critical error"),
                          QObject::tr("Failed to setup application data folders.\nExit application."));
    return -1;
  }

  QFontDatabase::addApplicationFont(":/fonts/LEMONMILK-Bold.otf");

  const QFont kBaseFont{"LEMONMILK"};
  QApplication::setFont(kBaseFont);

  cus::TextPlayer::Init();
  cus::TextPlayer::SetLocale(QLocale{"en"});
  cus::TextPlayer::SetRate(0.3);

  /* -- Updates -- */
  updt::RetrieveLatestDistantVersion(
      updt::GithubReleasesUrl("BrokenGameNoob", "BrokenTC22"),
      [](std::optional<updt::DistantVersion> version) {
        if (!version.has_value()) {
          SPDLOG_ERROR("Cannot retrieve latest version from github");
          return;
        }
        SPDLOG_INFO("Latest available version: {}", version->version);
      },
      true);

  /* -- Actual start -- */

  MainWindow w;
  w.show();
  return a.exec();
}
