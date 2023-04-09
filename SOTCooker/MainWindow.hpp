#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <WinUtils.hpp>
#include <QMainWindow>
#include <QPushButton>

#include <SOTCooker.pb.h>
#include <CookerProto.hpp>
#include <functional>
#include <unordered_map>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool SaveKeyboardProfile(){
        const auto kSuccess{pb::DumpProtobufToFile(m_config,c_config_file)};
        if(!kSuccess){
            qCritical() << "Failed to save config file at:" << c_config_file;
        }
#ifdef CMAKE_DEBUG_MODE
        else{
            qDebug() << "Saved config file at:" << c_config_file;
        }
#endif
        return kSuccess;
    }

private slots:
    void OnKeyboardPressed(int key);

private:
    sot::KeyboardProfile& GetCurrentProfile(){
        return m_config;
    }

    template<typename T>
    using KeyboardProfileSetter = void(sot::KeyboardProfile::*)(T);
    template<typename T>
    using KeyboardProfileGetter = T(sot::KeyboardProfile::*)()const;
    template<typename T>
    struct KeyboardProfileMutators{
        KeyboardProfileSetter<T> setter;
        KeyboardProfileGetter<T> getter;
    };

    void ConnectButton(QPushButton* pb,
                       KeyboardProfileMutators<int> mutators);
    void ConnectButton(QPushButton* pb,
                       KeyboardProfileSetter<int> setter,
                       KeyboardProfileGetter<int> getter){
        ConnectButton(pb,{setter,getter});
    }
    void ConnectButtons();

    void UpdateAllButtonsTexts();

private:
    Ui::MainWindow *ui;

    const QString c_appdata_folder;
    const QString c_config_file;

    sot::KeyboardProfile m_config{};
    std::unordered_map<QPushButton*,KeyboardProfileMutators<int>> m_pb_links{};
};
#endif // MAINWINDOW_HPP
