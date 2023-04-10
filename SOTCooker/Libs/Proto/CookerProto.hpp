#pragma once

#include "SOTCookerDefaults.hpp" //NOLINT
#include "ProtoHelp.hpp"//NOLINT
#include "CookerProtoConstants.hpp"//NOLINT

namespace sot{

inline
QString GetEnumValueName(sot::CookingType val){
    return QString::fromStdString(CookingType_Name(val));
}

/*!
 * Lmabda_t : void(int32_t key,sot::CookingType)
*/
template<typename Lambda_t>
int32_t ForEachKeyWithCookingType(const sot::KeyboardProfile& msg,Lambda_t to_apply){
    int32_t match_count{};

    std::function<bool(const google::protobuf::FieldDescriptor& desc, const google::protobuf::Reflection& reflection)> internal_to_apply =
    [&msg,&match_count,to_apply](const google::protobuf::FieldDescriptor& desc, const google::protobuf::Reflection& reflection)->bool{
        auto isKey{pb::GetFieldOption<sot::KeyboardProfile>(desc.number(),sot::is_keyboard_key)};
        if(!isKey){
            return false;
        }
        const auto kCookingType{pb::GetFieldOption<sot::KeyboardProfile>(desc.number(),sot::cooking_type)};
        if(kCookingType == sot::CookingType::kNone){
            return false;
        }
        ++match_count;

        to_apply(reflection.GetInt32(msg,&desc),kCookingType);

        return false;
    };
    pb::ApplyOnPbFields(&msg,internal_to_apply);
    return match_count;
}

}//namespace sot
