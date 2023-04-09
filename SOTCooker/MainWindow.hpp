#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include <SOTCooker.pb.h>
#include <CookerProto.hpp>

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
        return kSuccess;
    }

private slots:
    void OnKeyboardPressed(int key);

private:
    Ui::MainWindow *ui;

    const QString c_appdata_folder;
    const QString c_config_file;

    sot::KeyboardProfile m_config{};
};
#endif // MAINWINDOW_HPP
