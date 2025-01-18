#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QPushButton>
#include <SOTCooker.hpp>
#include <WinUtils.hpp>
#include <functional>
#include <unordered_map>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  const QColor kBaseBlueColor{25, 180, 255};

  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

  bool SaveKeyboardProfile() {
    const auto kSuccess{sot::SaveKeyboardProfile(m_config, c_config_file)};
    if (!kSuccess) {
      qCritical() << "Failed to save config file at:" << c_config_file;
    }
#ifdef CMAKE_DEBUG_MODE
    else {
      qDebug() << "Saved config file at:" << c_config_file;
    }
#endif
    return kSuccess;
  }

 private slots:
  void OnKeyboardPressed(int key);

  void OnCookerStarted(const sot::CookingType type);
  void OnCookerProgress(const double kPercentage);
  void OnCookerFinished(const sot::CookingType type);
  void OnCookerCancelled(const sot::CookingType type);

  void OnGetRemainingTimeRequested();

  void on_action_check_updates_triggered();

 private:
  sot::KeyboardProfile& GetCurrentProfile() {
    return m_config;
  }

  using KeyboardProfileKeyPtr = sot::KeyboardProfile::Key sot::KeyboardProfile::*;

  void ConnectButton(QPushButton* pb, KeyboardProfileKeyPtr key_ptr);
  void ConnectButtons();

  void UpdateAllButtonsTexts();

 private:
  Ui::MainWindow* ui;

  const QString c_appdata_folder;
  const QString c_config_file;

  sot::KeyboardProfile m_config{};
  std::unordered_map<QPushButton*, KeyboardProfileKeyPtr> m_pb_links{};

  sot::Cooker m_cooker{this};
};
#endif  // MAINWINDOW_HPP
