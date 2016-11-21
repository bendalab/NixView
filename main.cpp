#include "MainWindow.hpp"
#include <QApplication>
#include "nixview.h"
#include <QCommandLineParser>

int main(int argc, char *argv[]) {
    NixView a(argc, argv);
    MainWindow w(0, &a);

    QCommandLineParser parser;
    std::string help = "\nNixView --- viewer for nix data files.\n\nThis software is open-source published under the BSD3 license \nwithout any warranty. Source code can be found here:\nhttps://github.com/bendalab/nixview \n\nVisit https://github.com/g-node/nix for more information.";
    parser.setApplicationDescription(QString::fromStdString(help));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", QCoreApplication::translate("main", "the nix data file to open"));

    parser.process(a);
    const QStringList args = parser.positionalArguments();

    w.show();
    if (args.length() > 0) {
        w.read_nix_file(args[0]);
    }
    return a.exec();
}
