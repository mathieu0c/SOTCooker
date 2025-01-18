#include <QFile>
#include <QTextStream>

#include <Utils/xml_utils.hpp>

namespace xml {

QList<QDomElement> FindElement(QDomNode parent, const QString& subNode,
                               std::function<bool(const QDomElement&)> predicate) {
  QList<QDomElement> out{};

  while (!parent.isNull()) {
    auto children{parent.childNodes()};
    for (int i{}; i < children.size(); ++i) {
      auto child{children.at(i).toElement()};
      if (child.isNull()) break;

      if (child.tagName() == subNode) {
        if (predicate(child)) out.append(child);
      }
    }

    parent = parent.nextSibling();
  }

  return out;
}

std::optional<QDomDocument> GetXmlDoc(const QString& filePath) {
  QDomDocument doc{filePath};
  QFile f{filePath};

  if (!f.open(QIODevice::ReadOnly)) return {};
  if (!doc.setContent(&f)) {
    f.close();
    return {};
  }
  f.close();

  return doc;
}

bool SaveXml(const QDomDocument& doc, const QString& filePath) {
  QFile f{filePath};
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    return false;
  }

  QTextStream stream{&f};
  stream << doc.toString();

  f.close();
  return true;
}

}  // namespace xml
