#include "MainWindow.hpp"
#include <QApplication>
#include "nixview.h"

int main(int argc, char *argv[])
{
    NixView a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
