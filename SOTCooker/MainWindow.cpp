#include "MainWindow.hpp"
#include "./ui_MainWindow.h"

#include "GraphicsUtils.hpp"
#include "CircularProgressBar.hpp"

#include <WinUtils.hpp>
#include <WinEventHandler.hpp>
#include <DialogGetKeyCode.hpp>
#include <TextToSpeech.hpp>
#include <SOTCooker.hpp>
#include <SOTCookerUtils.hpp>

#include <QStandardPaths>
#include <QFileInfo>
#include <QMessageBox>

#include <QDebug>

namespace{

void test(){

}

}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      c_appdata_folder{QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/"},
      c_config_file{QString{"%0config.json"}.arg(c_appdata_folder)},
      m_updateHandler{new updt::UpdateHandler(consts::CURRENT_VERSION,consts::PROJECT_GITHUB_RELEASE,
                                              consts::PUBLIC_VERIFIER_KEY_FILE,true,consts::POST_UPDATE_CMD,
                                              true,this)}
{
    ui->setupUi(this);

    if(!QFileInfo::exists(c_config_file)){
        qInfo() << "Saving default keyboard profile";
        SaveKeyboardProfile();
    }

    test();

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
    connect(&m_cooker,&sot::Cooker::FinishedCooking,this,&MainWindow::OnCookerFinished);
    connect(&m_cooker,&sot::Cooker::CookingCancelled,this,&MainWindow::OnCookerCancelled);

    std::function<void(void)> nothing{[](){}};
    const auto kWasUpdated{updt::acquireUpdated(nothing,consts::UPDATED_TAG_FILENAME)};
    qInfo() << "Was updated?" << kWasUpdated;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnKeyboardPressed(int key){
    sot::ForEachKeyWithCookingType(GetCurrentProfile(),[&](int32_t action_key,sot::CookingType cooking_type){
        if(key != action_key){
            return;
        }

        qInfo() << "Start cooking!";
        m_cooker.StartCooking(cooking_type);
    });

    if(key == GetCurrentProfile().key_get_remaining_time()){
        OnGetRemainingTimeRequested();
    } else if(key == GetCurrentProfile().key_cancel_cooking()){
        m_cooker.Cancel();
    }
}

void MainWindow::OnCookerStarted(const sot::CookingType type){
    const auto kTypeStr{GetTypeHumanStr(type)};
    ui->lbl_type->setText(kTypeStr);
    const QString kAudioNotification{tr("%0 started").arg(kTypeStr)};
    cus::TextPlayer::Play(kAudioNotification);
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

void MainWindow::OnCookerFinished(const sot::CookingType type){
    ui->cpb_cooking->SetPercentage(100.);
    const auto kRemaining{QTime::fromMSecsSinceStartOfDay(0)};
    const auto kTimeStr{kRemaining.toString("mm:ss")};
    ui->lbl_remaining_time->setText(kTimeStr);

    const auto kTypeStr{GetTypeHumanStr(type)};
    const QString kAudioNotification{tr("%0 ready!").arg(kTypeStr)};
    cus::TextPlayer::Play(kAudioNotification);
}

void MainWindow::OnCookerCancelled(const sot::CookingType type){
    ui->cpb_cooking->SetPercentage(100.);
    const auto kRemaining{QTime::fromMSecsSinceStartOfDay(0)};
    const auto kTimeStr{kRemaining.toString("mm:ss")};
    ui->lbl_remaining_time->setText(kTimeStr);

    const auto kTypeStr{GetTypeHumanStr(type)};
    const QString kAudioNotification{tr("%0 cancelled!").arg(kTypeStr)};
    cus::TextPlayer::Play(kAudioNotification);
}


void MainWindow::OnGetRemainingTimeRequested(){
    const auto kRemaining{m_cooker.Timer().GetRemainingTime()};
    if(!kRemaining.isValid()){
        return;
    }
    cus::TextPlayer::Play(sot::GetAudioTimeStr(kRemaining));
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

    ConnectButton(ui->pb_key_start_cook_fish,
                  {&sot::KeyboardProfile::set_key_start_cooking_fish,
                  &sot::KeyboardProfile::key_start_cooking_fish});

    ConnectButton(ui->pb_key_start_cook_trophyfish,
                  {&sot::KeyboardProfile::set_key_start_cooking_trophy_fish,
                  &sot::KeyboardProfile::key_start_cooking_trophy_fish});

    ConnectButton(ui->pb_key_start_cook_meat,
                  {&sot::KeyboardProfile::set_key_start_cooking_meat,
                  &sot::KeyboardProfile::key_start_cooking_meat});

    ConnectButton(ui->pb_key_start_cook_kraken,
                  {&sot::KeyboardProfile::set_key_start_cooking_kraken,
                  &sot::KeyboardProfile::key_start_cooking_kraken});

    ConnectButton(ui->pb_key_start_cook_megalodon,
                  {&sot::KeyboardProfile::set_key_start_cooking_megalodon,
                  &sot::KeyboardProfile::key_start_cooking_megalodon});


    ConnectButton(ui->pb_key_start_cook_cycle,
                  {&sot::KeyboardProfile::set_key_start_cooking_cycle,
                  &sot::KeyboardProfile::key_start_cooking_cycle});


    ConnectButton(ui->pb_key_start_cook_cache,
                  {&sot::KeyboardProfile::set_key_start_cooking_cache,
                  &sot::KeyboardProfile::key_start_cooking_cache});


    ConnectButton(ui->pb_key_get_remaining_time,
                  {&sot::KeyboardProfile::set_key_get_remaining_time,
                  &sot::KeyboardProfile::key_get_remaining_time});

    ConnectButton(ui->pb_key_cancel,
                  {&sot::KeyboardProfile::set_key_cancel_cooking,
                  &sot::KeyboardProfile::key_cancel_cooking});
}

void MainWindow::UpdateAllButtonsTexts(){
    for(auto& [pb,mutators] : m_pb_links){
        UpdateButtonText(pb,std::invoke(mutators.getter,GetCurrentProfile()));
    }
}

void MainWindow::on_action_check_updates_triggered()
{
    m_updateHandler->show();
}

