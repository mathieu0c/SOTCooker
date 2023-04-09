/*!
 * Some code comes from the AntiMicroX project
*/

#pragma once

#include <QString>

#include <windows.h>

#include <vector>

namespace win {

//GetCurrentProcessId()

std::string getProcessName(DWORD processID);

bool terminateProcess(DWORD processID);

std::optional<DWORD> findProcessId(const QString& pName);
std::vector<DWORD> findProcessesId(const QString& pName);
std::vector<DWORD> getAllProcesses();


inline
bool isProcessRunning(const QString& pName){
    return findProcessId(pName).has_value();
}

inline
bool isProcessRunning(DWORD pId){
    auto ids{getAllProcesses()};
    return std::find(cbegin(ids),cend(ids),pId) != cend(ids);
}

int32_t processCount(const QString& pName);

int32_t getCoreCount();

bool setCoreCountAffinity(int32_t coreCountToUse,bool throwOnFail = false,bool verbose = false);


/* --- Keyboard --- */

constexpr unsigned int EXTENDED_FLAG = 0x100;

QString vkCodeToStr(int32_t keyCode);

void sendKeyboardEvent(int code, bool pressed,int codeAlias = 0);

// -- From AntiMicroX
QString virtualKeyCodeToString(UCHAR virtualKey); //not tested in this context

unsigned int scancodeFromVirtualKey(unsigned int virtualkey, unsigned int alias = 0);

unsigned int correctVirtualKey(unsigned int scancode, unsigned int virtualkey);

} // namespace win

