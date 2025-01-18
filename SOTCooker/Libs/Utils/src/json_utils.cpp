#include <QFile>
#include <QJsonValue>
#include <QString>
#include <QTextStream>

#include <Logger/logger.hpp>
#include <Utils/json_utils.hpp>

namespace utils {

bool Save(const QJsonDocument& doc, const QString& filename) {
  auto json_formatted(QString(doc.toJson()));
  QFile f(filename);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    return false;
  }

  QTextStream stream(&f);
  stream << json_formatted;
  f.close();
  return true;
}

std::optional<QJsonDocument> Read(const QString& filename) {
  QFile f(filename);
  if (!f.open(QIODevice::ReadOnly)) {
    return {};
  }

  auto json_bytes{f.readAll()};
  f.close();
  auto doc_read{QJsonDocument::fromJson(json_bytes)};
  return {doc_read};
}

}  // namespace utils
