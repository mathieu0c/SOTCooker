#include "MainWindow.hpp"
#include "./ui_MainWindow.h"

#include "GraphicsUtils.hpp"
#include "CircularProgressBar.hpp"

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

    const auto kPbOpt{pb::ReadFromFile<sot::KeyboardProfile>(c_config_file)};
    if(!kPbOpt && false){
        QString err{tr("Could not read config file: %0").arg(c_config_file)};
        qCritical() << err;
        QMessageBox::critical(this,tr("Error"),tr("Fatal error:\n%0").arg(err));
        throw std::runtime_error(err.toStdString());
    }
    GetCurrentProfile() = kPbOpt.value();

    connect(win::WindowsEventThread::ins(),&win::WindowsEventThread::keyDown,this,&MainWindow::OnKeyboardPressed);

//    win::sendKeyboardEvent(90,true);
//    win::sendKeyboardEvent(90,false);
//    auto key{win::DialogGetKeyCode::GetSimpleKey(this)};
//    if(key == win::DialogGetKeyCode::KeyVal::kCancelled){
//        qDebug() << "Cancelled";
//    } else if(key == win::DialogGetKeyCode::KeyVal::kUnbind){
//        qDebug() << "Unbind";
//    } else{
//        qDebug() << "Register key:" << key << " :" << win::virtualKeyCodeToString(static_cast<uint32_t>(key));
//    }

    ConnectButtons();
    UpdateAllButtonsTexts();

    connect(&m_cooker,&sot::Cooker::Progress,this,&MainWindow::OnCookerProgress);
    connect(&m_cooker,&sot::Cooker::StartedCooking,this,&MainWindow::OnCookerStarted);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnKeyboardPressed(int key){
    if(key == GetCurrentProfile().key_start_cooking()){
        qInfo() << "Start cooking!";
        m_cooker.StartCooking(sot::CookingType::kFish);
    }
}

void MainWindow::OnCookerStarted(const sot::CookingType type){
    ui->lbl_type->setText(GetTypeHumanStr(type));
}

void MainWindow::OnCookerProgress(const double kPercentage){
    ui->cpb_cooking->SetPercentage(kPercentage);
    const auto kRemaining{m_cooker.Timer().GetRemainingTime()};
    if(!kRemaining.isValid()){
        return;
    }
    const auto kTimeStr{kRemaining.toString("mm:ss")};
    ui->lbl_remaining_time->setText(kTimeStr);
}



void MainWindow::ConnectButton(QPushButton* pb,
                               KeyboardProfileMutators<int> mutators){
    connect(pb,&QPushButton::clicked,this,[this,pb,mutators]{
        auto key{win::DialogGetKeyCode::GetSimpleKey(this)};
        if(key == win::DialogGetKeyCode::KeyVal::kCancelled){
            return;
        } //else
        //note: an unbinded key value is win::DialogGetKeyCode::KeyVal::kUnbind = -1
        const auto& setter{mutators.setter};
        const auto& getter{mutators.getter};
        if(key == win::DialogGetKeyCode::KeyVal::kUnbind){
            std::invoke(setter,GetCurrentProfile(),sot::kUnbindKey);
        } else{
            std::invoke(setter,GetCurrentProfile(),key);
        }
        SaveKeyboardProfile();
        UpdateButtonText(pb,std::invoke(getter,GetCurrentProfile()));
    });
    m_pb_links[pb] = mutators;
}

void MainWindow::ConnectButtons(){
    ConnectButton(ui->pb_key_start_cooking,
                  {&sot::KeyboardProfile::set_key_start_cooking,
                  &sot::KeyboardProfile::key_start_cooking});
}

void MainWindow::UpdateAllButtonsTexts(){
    for(auto& [pb,mutators] : m_pb_links){
        UpdateButtonText(pb,std::invoke(mutators.getter,GetCurrentProfile()));
    }
}
