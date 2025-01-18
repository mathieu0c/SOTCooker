#include "MainWindow.hpp"

#include <DataStructures.hpp>
#include <DialogGetKeyCode.hpp>
#include <PathUtils.hpp>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <SOTCooker.hpp>
#include <SOTCookerUtils.hpp>
#include <TextToSpeech.hpp>
#include <WinEventHandler.hpp>
#include <WinUtils.hpp>
#include <git_version.hpp>

#include <UpdateTools/network.hpp>
#include <UpdateTools/requests.hpp>

#include "./ui_MainWindow.h"
#include "CircularProgressBar.hpp"
#include "GraphicsUtils.hpp"

namespace {

void test() {}

}  // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      c_appdata_folder{QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/"},
      c_config_file{QString{"%0config.json"}.arg(c_appdata_folder)}

{
  ui->setupUi(this);

  if (!QFileInfo::exists(c_config_file)) {
    qInfo() << "Saving default keyboard profile";
    SaveKeyboardProfile();
  }

  test();

  const auto kProfileOpt{sot::LoadKeyboardProfile(c_config_file)};
  if (!kProfileOpt && false) {
    QString err{tr("Could not read config file: %0").arg(c_config_file)};
    qCritical() << err;
    QMessageBox::critical(this, tr("Error"), tr("Fatal error:\n%0").arg(err));
    throw std::runtime_error(err.toStdString());
  }
  GetCurrentProfile() = kProfileOpt.value();

  connect(win::WindowsEventThread::ins(), &win::WindowsEventThread::keyDown, this, &MainWindow::OnKeyboardPressed);

  ConnectButtons();
  UpdateAllButtonsTexts();

  connect(&m_cooker, &sot::Cooker::Progress, this, &MainWindow::OnCookerProgress);
  connect(&m_cooker, &sot::Cooker::StartedCooking, this, &MainWindow::OnCookerStarted);
  connect(&m_cooker, &sot::Cooker::FinishedCooking, this, &MainWindow::OnCookerFinished);
  connect(&m_cooker, &sot::Cooker::CookingCancelled, this, &MainWindow::OnCookerCancelled);

  QLabel* status_version{new QLabel(QString::fromStdString(gitversion::GetVersionRepresentationString()))};
  auto version_font{status_version->font()};
  version_font.setBold(false);
  version_font.setPointSize(10);
  status_version->setFont(version_font);
  status_version->setStyleSheet("color: #000000;");
  ui->statusbar->addPermanentWidget(status_version);
}

MainWindow::~MainWindow() {
  delete ui;
}

struct KeyboardAction {
  sot::KeyboardProfile::Key sot::KeyboardProfile::*key_ptr{nullptr};
  std::function<void()> callback;
};

void MainWindow::OnKeyboardPressed(int key) {
  static const std::array kKeyboardActions{
      KeyboardAction{&sot::KeyboardProfile::start_cooking,
                     [this]() {
                       m_cooker.StartCooking(m_cooker.CookingType() == sot::CookingType::kNone
                                                 ? sot::CookingType::kFish
                                                 : m_cooker.CookingType());
                     }},
      KeyboardAction{&sot::KeyboardProfile::start_cooking_fish,
                     [this]() { m_cooker.StartCooking(sot::CookingType::kFish); }},
      KeyboardAction{&sot::KeyboardProfile::start_cooking_trophy_fish,
                     [this]() { m_cooker.StartCooking(sot::CookingType::kTrophyFish); }},
      KeyboardAction{&sot::KeyboardProfile::start_cooking_meat,
                     [this]() { m_cooker.StartCooking(sot::CookingType::kMeat); }},
      KeyboardAction{&sot::KeyboardProfile::start_cooking_kraken,
                     [this]() { m_cooker.StartCooking(sot::CookingType::kKraken); }},
      KeyboardAction{&sot::KeyboardProfile::start_cooking_megalodon,
                     [this]() { m_cooker.StartCooking(sot::CookingType::kMegalodon); }},
      KeyboardAction{&sot::KeyboardProfile::start_cooking_cycle,
                     [this]() { m_cooker.StartCooking(sot::LoopCookingType(m_cooker.CookingType(), true)); }},
      KeyboardAction{&sot::KeyboardProfile::start_cooking_cache,
                     [this]() { m_cooker.StartCooking(sot::CookingType::kCache); }},
      KeyboardAction{&sot::KeyboardProfile::get_remaining_time, [this]() { OnGetRemainingTimeRequested(); }},
      KeyboardAction{&sot::KeyboardProfile::cancel_cooking, [this]() { m_cooker.Cancel(); }},
  };

  for (const auto& action : kKeyboardActions) {
    if (key == GetCurrentProfile().*action.key_ptr) {
      action.callback();
    }
  }
}

void MainWindow::OnCookerStarted(const sot::CookingType type) {
  const auto kTypeStr{GetName(type)};
  ui->lbl_type->setText(kTypeStr);
  const QString kAudioNotification{tr("%0 started").arg(kTypeStr)};
  cus::TextPlayer::Play(kAudioNotification);
}

void MainWindow::OnCookerProgress(const double kPercentage) {
  ui->cpb_cooking->SetPercentage(kPercentage);
  const auto kRemaining{m_cooker.Timer().GetRemainingTime()};
  if (!kRemaining.isValid()) {
    return;
  }
  const auto kTimeStr{kRemaining.toString("mm:ss")};
  ui->lbl_remaining_time->setText(kTimeStr);
}

void MainWindow::OnCookerFinished(const sot::CookingType type) {
  ui->cpb_cooking->SetPercentage(100.);
  const auto kRemaining{QTime::fromMSecsSinceStartOfDay(0)};
  const auto kTimeStr{kRemaining.toString("mm:ss")};
  ui->lbl_remaining_time->setText(kTimeStr);

  const auto kTypeStr{GetName(type)};
  const QString kAudioNotification{tr("%0 ready!").arg(kTypeStr)};
  cus::TextPlayer::Play(kAudioNotification);
}

void MainWindow::OnCookerCancelled(const sot::CookingType type) {
  ui->cpb_cooking->SetPercentage(100.);
  const auto kRemaining{QTime::fromMSecsSinceStartOfDay(0)};
  const auto kTimeStr{kRemaining.toString("mm:ss")};
  ui->lbl_remaining_time->setText(kTimeStr);

  const auto kTypeStr{GetName(type)};
  const QString kAudioNotification{tr("%0 cancelled!").arg(kTypeStr)};
  cus::TextPlayer::Play(kAudioNotification);
}

void MainWindow::OnGetRemainingTimeRequested() {
  const auto kRemaining{m_cooker.Timer().GetRemainingTime()};
  if (!kRemaining.isValid()) {
    return;
  }
  cus::TextPlayer::Play(sot::GetAudioTimeStr(kRemaining));
}

void MainWindow::ConnectButton(QPushButton* pb, KeyboardProfileKeyPtr key_ptr) {
  connect(pb, &QPushButton::clicked, this, [this, pb, key_ptr] {
    auto key{win::DialogGetKeyCode::GetSimpleKey(this)};
    if (key == win::DialogGetKeyCode::KeyVal::kCancelled) {
      return;
    }  // else

    if (key == win::DialogGetKeyCode::KeyVal::kUnbind) {
      GetCurrentProfile().*key_ptr = sot::KeyboardProfile::kUnbindKey;
    } else {
      GetCurrentProfile().*key_ptr = key;
    }
    SaveKeyboardProfile();
    UpdateButtonText(pb, GetCurrentProfile().*key_ptr);
  });
  m_pb_links[pb] = key_ptr;
}

void MainWindow::ConnectButtons() {
  ConnectButton(ui->pb_key_start_cooking, &sot::KeyboardProfile::start_cooking);
  ConnectButton(ui->pb_key_start_cook_fish, &sot::KeyboardProfile::start_cooking_fish);
  ConnectButton(ui->pb_key_start_cook_trophyfish, &sot::KeyboardProfile::start_cooking_trophy_fish);
  ConnectButton(ui->pb_key_start_cook_meat, &sot::KeyboardProfile::start_cooking_meat);
  ConnectButton(ui->pb_key_start_cook_kraken, &sot::KeyboardProfile::start_cooking_kraken);
  ConnectButton(ui->pb_key_start_cook_megalodon, &sot::KeyboardProfile::start_cooking_megalodon);
  ConnectButton(ui->pb_key_start_cook_cycle, &sot::KeyboardProfile::start_cooking_cycle);
  ConnectButton(ui->pb_key_start_cook_cache, &sot::KeyboardProfile::start_cooking_cache);
  ConnectButton(ui->pb_key_get_remaining_time, &sot::KeyboardProfile::get_remaining_time);
  ConnectButton(ui->pb_key_cancel, &sot::KeyboardProfile::cancel_cooking);
}

void MainWindow::UpdateAllButtonsTexts() {
  for (auto& [pb, key_ptr] : m_pb_links) {
    UpdateButtonText(pb, GetCurrentProfile().*key_ptr);
  }
}

void MainWindow::on_action_check_updates_triggered() {
  sot::RetrieveUpdate(true);
}
