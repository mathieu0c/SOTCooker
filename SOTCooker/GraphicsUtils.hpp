#pragma once

#include <DataStructures.hpp>
#include <QPushButton>
#include <WinUtils.hpp>

inline void UpdateButtonText(QPushButton* pb, int32_t key) {
  if (key == sot::KeyboardProfile::kUnbindKey) {
    pb->setText("");
    return;
  }
  pb->setText(win::GetVkStr(key));
}
