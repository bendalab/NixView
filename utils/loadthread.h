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
    LoadThread(QObject *parent = 0, unsigned int chunksize=1000000);
    ~LoadThread();

    void run() override;
    void setVariables(const nix::DataArray &array, nix::NDSize start, nix::NDSize extent, std::vector<int> index2D, unsigned int xDim, int graphIndex);
    void setVariables1D(const nix::DataArray &array, nix::NDSize start, nix::NDSize extent, int graphIndex);
    void setChuncksize(unsigned int size);

private:
    void getAxis(const nix::DataArray &array, QVector<double> &axis, unsigned int count, unsigned int offset, int xDim);
    bool testInput(const nix::DataArray &array, nix::NDSize start, nix::NDSize extent);
    void load1D(nix::DataArray array, nix::NDSize start, nix::NDSize extent, unsigned int chunksize, int graphIndex);
    void load2D(nix::DataArray array, nix::NDSize start, nix::NDSize extent,  unsigned int xDim, std::vector<int> index2D, unsigned int chunksize, int graphIndex);

signals:
    void dataReady(const QVector<double> &data, const QVector<double> &axis, int index);
    void progress(double percent, int index);

private:
    QMutex mutex;
    QWaitCondition condition;
    nix::DataArray array;
    nix::NDSize start;
    nix::NDSize extent;
    unsigned int chunksize;
    unsigned int xDim;
    std::vector<int> index2D;
    int graphIndex;
    bool abort;
    bool restart;
};

#endif // LOADTHREAD_H
