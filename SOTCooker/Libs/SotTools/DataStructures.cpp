#include "DataStructures.hpp"

#include <QFile>
#include <QJsonDocument>
#include <QObject>
#include <QTextStream>
#include <array>
#include <optional>

namespace sot {

const CookingTypeSettings& GetCookingSettings(CookingType type) {
  static const std::array kCookingSettings{
      CookingTypeSettings{false, 0, QObject::tr("Uknown")},      /* kNone */
      CookingTypeSettings{true, 44, QObject::tr("Fish")},        /* kFish */
      CookingTypeSettings{true, 94, QObject::tr("Trophy fish")}, /* kTrophyFish */
      CookingTypeSettings{true, 64, QObject::tr("Meat")},        /* kMeat */
      CookingTypeSettings{true, 124, QObject::tr("Kraken")},     /* kKraken */
      CookingTypeSettings{true, 124, QObject::tr("Megalodon")},  /* kMegalodon */
      CookingTypeSettings{true, 179, QObject::tr("Cache")},      /* kCache */
  };

  static_assert(kCookingSettings.size() == static_cast<size_t>(CookingType::kMaxValue),
                "Missing value in CookingTypeSettings");

  if (Underlying(type) < 0 || Underlying(type) >= Underlying(CookingType::kMaxValue)) {
    return kCookingSettings[0];
  }

  return kCookingSettings[static_cast<size_t>(type)];
}

int32_t GetCookingMs(CookingType type) {
  return GetCookingSettings(type).cooking_time * 1000;
}

const QString& GetName(CookingType type) {
  return GetCookingSettings(type).name;
}

CookingType LoopCookingType(CookingType type, bool move_forward) {
  const auto kNext{Underlying(type) + (move_forward ? 1 : -1)};

  /* Exception for looping back to top when going backward with a bottom value */
  if (kNext <= Underlying(CookingType::kNone) && Underlying(type) >= Underlying(CookingType::kFish)) {
    type = static_cast<CookingType>(Underlying(CookingType::kMaxValue) - 1);
  }

  if (kNext >= Underlying(CookingType::kMaxValue) || kNext <= Underlying(CookingType::kNone)) {
    type = static_cast<CookingType>(Underlying(CookingType::kNone) + 1);
  }

  return static_cast<CookingType>(type);
}

QJsonObject ToJson(const KeyboardProfile& profile) {
  QJsonObject obj{};
  obj["start_cooking"] = profile.start_cooking;
  obj["start_cooking_fish"] = profile.start_cooking_fish;
  obj["start_cooking_trophy_fish"] = profile.start_cooking_trophy_fish;
  obj["start_cooking_meat"] = profile.start_cooking_meat;
  obj["start_cooking_kraken"] = profile.start_cooking_kraken;
  obj["start_cooking_megalodon"] = profile.start_cooking_megalodon;
  obj["start_cooking_cycle"] = profile.start_cooking_cycle;
  obj["start_cooking_cache"] = profile.start_cooking_cache;
  obj["get_remaining_time"] = profile.get_remaining_time;
  obj["cancel_cooking"] = profile.cancel_cooking;
  return obj;
}

KeyboardProfile FromJson(const QJsonObject& json) {
  KeyboardProfile profile{};
  profile.start_cooking = json["start_cooking"].toInt();
  profile.start_cooking_fish = json["start_cooking_fish"].toInt();
  profile.start_cooking_trophy_fish = json["start_cooking_trophy_fish"].toInt();
  profile.start_cooking_meat = json["start_cooking_meat"].toInt();
  profile.start_cooking_kraken = json["start_cooking_kraken"].toInt();
  profile.start_cooking_megalodon = json["start_cooking_megalodon"].toInt();
  profile.start_cooking_cycle = json["start_cooking_cycle"].toInt();
  profile.start_cooking_cache = json["start_cooking_cache"].toInt();
  profile.get_remaining_time = json["get_remaining_time"].toInt();
  profile.cancel_cooking = json["cancel_cooking"].toInt();
  return profile;
}

bool SaveKeyboardProfile(const KeyboardProfile& profile, const QString& file) {
  QJsonDocument doc{ToJson(profile)};
  auto json_formatted(QString(doc.toJson()));
  QFile f(file);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    return false;
  }

  QTextStream stream(&f);
  stream << json_formatted;
  f.close();
  return true;
}

std::optional<KeyboardProfile> LoadKeyboardProfile(const QString& file) {
  QFile f(file);
  if (!f.open(QIODevice::ReadOnly)) {
    return {};
  }

  auto json_bytes{f.readAll()};
  f.close();
  auto doc_read{QJsonDocument::fromJson(json_bytes)};
  return {FromJson(doc_read.object())};
}

}  // namespace sot
