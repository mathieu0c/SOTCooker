#pragma once

#include "SOTCooker.pb.h"

#include "ProtoHelp.hpp" //NOLINT

namespace pb {

//https://protobuf.dev/reference/cpp/api-docs/google.protobuf.message/#Reflection
template<>
inline
sot::KeyboardProfile GetDefault(){
    sot::KeyboardProfile out{};

    out.set_profile_name("Default");

    constexpr auto expectedKeyFieldCount{1};

    int32_t keyFieldCount{0};

    keyFieldCount = SetFieldTypeToValue<PbCppType::CPPTYPE_INT32>(&out,&PbReflection::SetInt32,-1);

    if(keyFieldCount != expectedKeyFieldCount)
    {
        throw std::runtime_error(std::string{__PRETTY_FUNCTION__}+" Invalid init for: "+out.descriptor()->full_name());
    }

    return out;
}

} // namespace pb
