#include "MainWindow.hpp"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFontDatabase>

#include "LoggerHandler.hpp"

int main(int argc, char *argv[])
{
    installCustomLogHandler(logHandler::GlobalLogInfo{.progLogFilePath="Cooker.log",.progName="SOTCooker"});

    QApplication a(argc, argv);

    QFontDatabase::addApplicationFont(":/fonts/LEMONMILK-Bold.otf");

    const QFont kBaseFont{"LEMONMILK"};
    QApplication::setFont(kBaseFont);

    MainWindow w;
    w.show();
    return a.exec();
}
