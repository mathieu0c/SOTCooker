#pragma once

#include <QString>
#include <git_version.hpp>

#include <UpdateTools/network.hpp>
#include <UpdateTools/requests.hpp>

namespace sot {

QString GetAppDataPath();

void RetrieveUpdate(bool display_info_if_no_update_advised);
void OnUpdateRetrieved(std::optional<updt::DistantVersion> version, bool display_info_if_no_update_advised);

}  // namespace sot
