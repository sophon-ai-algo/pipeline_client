#include "stream_demo_qt.h"
#include <QtWidgets/QApplication>
#include <qstylefactory.h>
#include <fstream>
#include "json/json.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication a(argc, argv);
    stream_demo_qt w;
    w.show();
    return a.exec();
}
