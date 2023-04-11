#pragma once

#include <CookerProto.hpp>

namespace sot{

inline
int32_t GetCookingMs(sot::CookingType type){
    const auto* kEnumDesc{google::protobuf::GetEnumDescriptor<decltype(type)>()};

    const auto kSeconds{kEnumDesc->FindValueByNumber(type)->options().GetExtension(sot::cooking_time)};

#ifdef CMAKE_DEBUG_MODE
    return static_cast<int32_t>(kSeconds*20.);
#else
    return static_cast<int32_t>(kSeconds*1000.);
#endif
}

inline
QString GetTypeHumanStr(sot::CookingType type){
    return GetEnumValueName(type).mid(1);
}

}//namespace sot
