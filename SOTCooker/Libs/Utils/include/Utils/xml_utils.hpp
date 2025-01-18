#pragma once

#include <QDomElement>
#include <QDomNode>
#include <QList>
#include <optional>

namespace xml {

QList<QDomElement> FindElement(
    QDomNode parent, const QString& subNode,
    std::function<bool(const QDomElement&)> predicate = [](const auto&) { return true; });

std::optional<QDomDocument> GetXmlDoc(const QString& filePath);

inline QDomElement GetXmlRoot(const QDomDocument& doc) {
  return doc.documentElement();
}

bool SaveXml(const QDomDocument& doc, const QString& filePath);

}  // namespace xml
