#pragma once

#include <SimpleUpdater.hpp>

namespace consts{

constexpr auto PROJECT_GITHUB_RELEASE{PROJECT_GITHUB_RELEASE_DEFINE};
constexpr updt::Version CURRENT_VERSION{PROJECT_V_MAJOR,PROJECT_V_MINOR,PROJECT_V_PATCH};

constexpr auto PUBLIC_VERIFIER_KEY_FILE{"Assets/SOTCooker.public"};

constexpr auto POST_UPDATE_CMD{"SOTCooker.exe"};

constexpr auto UPDATED_TAG_FILENAME{"UPDATED.TAG"};
}
