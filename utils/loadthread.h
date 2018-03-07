#ifndef LOADTHREAD_H
#define LOADTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <nix.hpp>


class LoadThread: public QThread
{
    Q_OBJECT

public:
    LoadThread();
    ~LoadThread();

    void run() override;
    bool setVariables(const nix::DataArray &array, nix::NDSize start, nix::NDSize extend);

private:
    bool testInput(const nix::DataArray &array, nix::NDSize start, nix::NDSize extend);

signals:
    void dataReady(QVector<double> data, nix::NDSize start, nix::NDSize extend);

private:
    QMutex mutex;
    QWaitCondition condition;
    nix::DataArray &array;
    nix::NDSize start;
    nix::NDSize extend;
    bool abort;

};

#endif // LOADTHREAD_H
