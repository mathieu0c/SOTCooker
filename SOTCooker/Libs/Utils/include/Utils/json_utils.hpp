#pragma once

#include <QJsonDocument>
#include <QJsonObject>

namespace utils {

inline QString ToString(const QJsonObject& obj) {
  return QJsonDocument(obj).toJson();
}

bool Save(const QJsonDocument& doc, const QString& fileName);

inline bool Save(const QJsonObject& obj, const QString& filename) {
  return Save(QJsonDocument(obj), filename);
}

std::optional<QJsonDocument> Read(const QString& fileName);

}  // namespace utils
