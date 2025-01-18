#include "PathUtils.hpp"

#include <QStandardPaths>

namespace sot {

QString GetAppDataPath() {
  return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

}  // namespace sot
