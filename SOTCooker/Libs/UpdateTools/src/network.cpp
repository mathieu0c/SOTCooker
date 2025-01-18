#include <QJsonDocument>

#include <Logger/logger.hpp>
#include <UpdateTools/network.hpp>

namespace updt {

QString GithubApiUrl(const QString& user, const QString& repo, const QString& endpoint) {
  return QString{"https://api.github.com/repos/%1/%2/%3"}.arg(user, repo, endpoint);
}

QString GithubReleasesUrl(const QString& user, const QString& repo) {
  return GithubApiUrl(user, repo, "releases");
}

void GetJsonFromAPI(const QString& url, std::function<void(std::optional<QJsonDocument>)> callback) {
  auto parent{new QObject{}};
  auto net_manager{new QNetworkAccessManager(parent)};
  QObject::connect(
      net_manager, &QNetworkAccessManager::finished, parent, [net_manager, callback, url, parent](QNetworkReply* rep) {
        if (rep->error() != QNetworkReply::NoError) {
          SPDLOG_ERROR("Cannot retrieve informations from {}:\n{}", url, rep->errorString());
          callback({});  // call callback with null optionnal = ERROR
        } else {
          auto rep_bytes{rep->readAll()};
          QJsonParseError converstionSuccess{};
          auto doc{QJsonDocument::fromJson(rep_bytes, &converstionSuccess)};
          if (converstionSuccess.error != QJsonParseError::NoError) {
            SPDLOG_ERROR("Cannot parse json retrieved from {}:\n{}", url, converstionSuccess.errorString());
            callback({});
          } else {
            callback({doc});
          }
        }
        net_manager->deleteLater();
        rep->deleteLater();
        parent->deleteLater();
      });
  net_manager->get(QNetworkRequest{QUrl{url}});
}

}  // namespace updt
