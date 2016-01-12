#ifndef NIXVIEW_H
#define NIXVIEW_H

#include <QApplication>

class NixView : public QApplication
{
public:
    NixView(int &argc, char** argv);

private:
    bool notify(QObject *receiver_, QEvent *event_);

};

#endif // NIXVIEW_H
