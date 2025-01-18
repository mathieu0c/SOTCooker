#pragma once

#include <QJsonObject>
#include <QString>
#include <compare>
#include <cstdint>

namespace sot {

enum class CookingType : int32_t {
  kNone,
  kFish,
  kTrophyFish,
  kMeat,
  kKraken,
  kMegalodon,
  kCache,
  kMaxValue,
};
constexpr auto Underlying(CookingType type) {
  return static_cast<int32_t>(type);
}
constexpr bool operator==(CookingType lhs, CookingType rhs) {
  return Underlying(lhs) == Underlying(rhs);
}

struct CookingTypeSettings {
  bool valid{};
  int32_t cooking_time{}; /* In s */
  QString name{};

  operator bool() const {
    return valid;
  }
};

struct KeyboardProfile {
  using Key = int32_t;
  static constexpr Key kUnbindKey{-1};

  Key start_cooking{};
  Key start_cooking_fish{};
  Key start_cooking_trophy_fish{};
  Key start_cooking_meat{};
  Key start_cooking_kraken{};
  Key start_cooking_megalodon{};

  Key start_cooking_cycle{};

  Key start_cooking_cache{};

  Key get_remaining_time{};
  Key cancel_cooking{};
};

const CookingTypeSettings& GetCookingSettings(CookingType type);
int32_t GetCookingMs(CookingType type);
const QString& GetName(CookingType type);
CookingType LoopCookingType(CookingType type, bool move_forward); /* move_forward == false => move_backward */

QJsonObject ToJson(const KeyboardProfile& profile);
KeyboardProfile FromJson(const QJsonObject& json);
bool SaveKeyboardProfile(const KeyboardProfile& profile, const QString& file);
std::optional<KeyboardProfile> LoadKeyboardProfile(const QString& file);

}  // namespace sot
