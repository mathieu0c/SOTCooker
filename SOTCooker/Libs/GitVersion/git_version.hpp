#pragma once

#include <string>

#include "git_version_structs.hpp"

namespace gitversion {

const GitInfo& GetInfo();

const std::string& GetVersionRepresentationString();

}  // namespace gitversion
