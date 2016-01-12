#include "nixview.h"
#include <iostream>

NixView::NixView(int &argc, char **argv) : QApplication(argc, argv){

}


bool NixView::notify(QObject *receiver_, QEvent *event_) {
    try {
        return QApplication::notify(receiver_, event_);
    } catch (std::exception &ex) {
        std::cerr << "std::exception was caught: \n" << ex.what() << std::endl;
    }
    return false;
}

