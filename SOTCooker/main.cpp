#include "MainWindow.hpp"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "LoggerHandler.hpp"

int main(int argc, char *argv[])
{
    installCustomLogHandler(logHandler::GlobalLogInfo{.progLogFilePath="Cooker.log",.progName="SOTCooker"});

    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}
