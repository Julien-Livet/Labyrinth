#include <ctime>

#include <QApplication>

#include "Labyrinth2d/Qt/MainWindow.h"

int main(int argc, char* argv[])
{
    srand(time(NULL));

    QApplication app(argc, argv);
    app.setApplicationName("Labyrinth");
    app.setApplicationVersion(QString("Version ") + VERSION);

    MainWindow mainWindow;

#if defined(Q_WS_MAC)
    if(QCoreApplication::arguments().count() > 1)
        QApplication::instance()->installEventFilter(&mainWindow);
#else
    if(QCoreApplication::arguments().count() > 1)
        mainWindow.chargerPartie(QCoreApplication::arguments().at(1));
#endif

    return app.exec();
}
