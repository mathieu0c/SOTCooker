#include "WinEventHandler.hpp"

#include <QTimer>

#include "WinUtils.hpp"

#include <QDebug>

namespace win {

LRESULT CALLBACK WindowsEventThread::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    BOOL fEatKeystroke = FALSE;

    if (nCode == HC_ACTION)
    {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
        auto vkCode = p->vkCode;
        switch (wParam)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            emitKeyDown(vkCode);
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
        default:
            break;
        }
    }
    return(fEatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}


WindowsEventThread::WindowsEventThread() : QObject()
{
    HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, WindowsEventThread::LowLevelKeyboardProc, 0, 0);
    std::ignore = hhkLowLevelKybd;
}

WindowsEventThread::~WindowsEventThread()
{

}

} // namespace win
