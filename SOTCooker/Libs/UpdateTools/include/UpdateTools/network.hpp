#pragma once

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QString>
#include <QStringList>

namespace updt {

QString GithubApiUrl(const QString& user, const QString& repo, const QString& endpoint);
QString GithubReleasesUrl(const QString& user, const QString& repo);

void GetJsonFromAPI(const QString& url, std::function<void(std::optional<QJsonDocument>)> callback);

}  // namespace updt
