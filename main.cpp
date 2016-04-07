#include "MainWindow.hpp"
#include <QApplication>
#include "nixview.h"

int main(int argc, char *argv[])
{
    NixView a(argc, argv);
    MainWindow w(0, &a);
    w.show();

    return a.exec();
}
