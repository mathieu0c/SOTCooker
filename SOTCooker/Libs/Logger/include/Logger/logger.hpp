#pragma once

#include <src/constants.hpp>

// include separator

#include <QStandardPaths>
#include <QString>

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

/*!
 * Declare a function protype with std::ostream "stream" and TypeName& "val"
 */
#define DECLARE_CUSTOM_SPD_FORMAT(TypeName)               \
  template <>                                             \
  struct fmt::formatter<TypeName> : ostream_formatter {}; \
  inline std::ostream &operator<<(std::ostream &stream, const QString &val)

DECLARE_CUSTOM_SPD_FORMAT(QString) {
  return stream << val.toStdString();
}

namespace logger {

inline QString GetLogFolderPath() {
  auto val{QString{"%0/Logs"}.arg(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))};
  return val;
}

inline QString GetLogFilePath(const QString &prog_name) {
  auto val{QString{"%0/%1Logs.txt"}.arg(GetLogFolderPath(), prog_name)};
  return val;
}

}  // namespace logger
