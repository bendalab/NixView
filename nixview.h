#ifndef NIXVIEW_H
#define NIXVIEW_H

#include <QApplication>

class NixView : public QApplication
{
    Q_OBJECT

public:
    NixView(int &argc, char** argv);

signals:
    void invalid_file_error();

private:
    bool notify(QObject *receiver_, QEvent *event_);
};

#endif // NIXVIEW_H
