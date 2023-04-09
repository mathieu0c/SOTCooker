#pragma once

#include <WinUtils.hpp>
#include <QPushButton>
#include <CookerProto.hpp>

inline
void UpdateButtonText(QPushButton* pb,int32_t key){
    if(key == sot::kUnbindKey){
        pb->setText("");
        return;
    }
    pb->setText(win::GetVkStr(key));
}
