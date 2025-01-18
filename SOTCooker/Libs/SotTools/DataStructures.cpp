#include "DataStructures.hpp"

#include <QObject>
#include <array>

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

}  // namespace sot
