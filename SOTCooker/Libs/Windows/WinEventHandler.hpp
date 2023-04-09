#pragma once

#include <QThread>
#include <windows.h>

namespace win {


class WindowsEventThread : public QObject
{
Q_OBJECT
public:
    static WindowsEventThread* instance()
    {
        static WindowsEventThread m_instance{};
        return &m_instance;
    }
    static WindowsEventThread* ins(){
        return instance();
    }

    void lemitKeyDown(int key){
        emit keyDown(key);
    }

    static void emitKeyDown(int key){
        ins()->lemitKeyDown(key);
    }

    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

signals:
    void keyDown(int key);

private:
    WindowsEventThread();
    ~WindowsEventThread();
    WindowsEventThread(const WindowsEventThread&)= delete;
    WindowsEventThread& operator=(const WindowsEventThread&)= delete;
};


} // namespace win
