#ifndef LOADTHREAD_H
#define LOADTHREAD_H

#include <QThread>


class LoadThread: public QThread
{
    Q_OBJECT

public:
    LoadThread();

    void run() override;

signals:

    void resultsReady();

};

#endif // LOADTHREAD_H
