#include "Eter_UI.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Eter_UI w;
    w.show();
    return a.exec();
}
