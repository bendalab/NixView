#include "nixview.h"
#include <iostream>
#include <nix.hpp>

NixView::NixView(int &argc, char **argv) : QApplication(argc, argv) {}

bool NixView::notify(QObject *receiver_, QEvent *event_) {
    try {
        return QApplication::notify(receiver_, event_);
    } catch (nix::InvalidFile) {
        emit invalid_file_error();
    } catch (std::exception &ex) {
        std::cerr << "std::exception was caught: \n" << ex.what() << std::endl;
    }
    return false;
}

