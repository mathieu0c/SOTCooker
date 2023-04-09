#pragma once

#include "SOTCookerDefaults.hpp"
#include "ProtoHelp.hpp"

namespace sot{

constexpr int kUnbindKey{-1};

inline
QString GetEnumValueName(sot::CookingType val){
    return QString::fromStdString(CookingType_Name(val));
}

}//namespace sot
