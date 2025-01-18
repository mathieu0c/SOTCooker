#pragma once

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

struct CookingTypeSettings {
  bool valid{};
  int32_t cooking_time{}; /* In s */
  QString name{};

  operator bool() const {
    return valid;
  }
};

const CookingTypeSettings& GetCookingSettings(CookingType type);
int32_t GetCookingMs(CookingType type);
const QString& GetName(CookingType type);
CookingType LoopCookingType(CookingType type, bool move_forward); /* move_forward == false => move_backward */

}  // namespace sot
