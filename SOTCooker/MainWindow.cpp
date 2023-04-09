#include "MainWindow.hpp"
#include "./ui_MainWindow.h"

#include <WinUtils.hpp>
#include <WinEventHandler.hpp>
#include <DialogGetKeyCode.hpp>

#include <QStandardPaths>
#include <QFileInfo>
#include <QMessageBox>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      c_appdata_folder{QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/"},
      c_config_file{QString{"%0config.json"}.arg(c_appdata_folder)}
{
    ui->setupUi(this);

    if(!QFileInfo::exists(c_config_file)){
        qInfo() << "Saving default keyboard profile";
        SaveKeyboardProfile();
    }

    const auto kPbOpt{pb::ReadFromFile<decltype(m_config)>(c_config_file)};
    if(!kPbOpt && false){
        QString err{tr("Could not read config file: %0").arg(c_config_file)};
        qCritical() << err;
        QMessageBox::critical(this,tr("Error"),tr("Fatal error:\n%0").arg(err));
        throw std::runtime_error(err.toStdString());
    }

    connect(win::WindowsEventThread::ins(),&win::WindowsEventThread::keyDown,this,&MainWindow::OnKeyboardPressed);

//    win::sendKeyboardEvent(90,true);
//    win::sendKeyboardEvent(90,false);
    auto key{win::DialogGetKeyCode::GetSimpleKey(this)};
    if(key == win::DialogGetKeyCode::KeyVal::kCancelled){
        qDebug() << "Cancelled";
    } else if(key == win::DialogGetKeyCode::KeyVal::kUnbind){
        qDebug() << "Unbind";
    } else{
        qDebug() << "Register key:" << key << " :" << win::virtualKeyCodeToString(static_cast<uint32_t>(key));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnKeyboardPressed(int key){
    qDebug() << "Key pressed:" << key;
}

