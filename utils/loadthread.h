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
    LoadThread(QObject *parent = 0, unsigned int chunksize =1000000);
    ~LoadThread();

    void run() override;
    void setVariables(const nix::DataArray &array, nix::NDSize start, nix::NDSize extend);
    void setChuncksize(unsigned int size);

private:
    void getAxis(const nix::DataArray &array, QVector<double> &axis, unsigned int count, unsigned int offset, int xDim);
    bool testInput(const nix::DataArray &array, nix::NDSize start, nix::NDSize extend);

signals:
    void dataReady(const QVector<double> &data);
    void progress(double percent);

private:
    QMutex mutex;
    QWaitCondition condition;
    nix::DataArray array;
    nix::NDSize start;
    nix::NDSize extend;
    unsigned int chunksize;
    bool abort;

};

#endif // LOADTHREAD_H
