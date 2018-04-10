#ifndef LOADTHREAD_H
#define LOADTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <nix.hpp>
#include "../plotter/plotter.h"


class LoadThread: public QThread
{
    Q_OBJECT

public:
    /**
     * @brief LoadThread: A small thread made to easily load data for plots outside of the guiThread.
     * @param parent
     * @param chunksize: The size the thread splits the work into. It emits the progress signal after each chunk.
     */
    LoadThread(QObject *parent = 0, unsigned int chunksize=1000000);
    ~LoadThread();

    void run() override;

    /**
     * @brief setVariables: The main way to interact with the thread. It sets all needed variables and starts the thread.
     * @param array: The array with the data to be loaded (currently max 2D)
     * @param start: nix::NDSize with the same dimensionality as the Array defining the offset/startIndex where the loading begins.
     * @param extent: nix::NDSize with the same dimensionality as the Array defining the length of data to get.
     * @param index2D: A vector of indicies that defines which indices of a 2D Array should be loaded. (An empty array means all)
     * @param dim: the nix dimension  that describes the xAxis.
     * @param dimNumber: The 'index' of the nix::Dimension to be used as the xAxis.
     * @param Index: An Index that will be givenin the signals to be able to use multiple threads at the same time.
     *          For 2D Arrays the Index is increassed by the index in second dimension of the loaded data.
     */
    void setVariables(const nix::DataArray &array, nix::NDSize start, nix::NDSize extent, nix::Dimension dim, std::vector<int> index2D, unsigned int dimNumber, int Index);

    /**
     * @brief setVariables1D: A smaller setVariables for 1D Arrays that don't need all members. Also starts the thread.
     * @param array: 1D DataArray to be loaded.
     * @param start: 1D nix::NDSize defining the startindex of the loading.
     * @param extent: 1D nix::NDSize defining the size of the part to be loaded.
     * @param dim: the nix dimension  that describes the xAxis.
     * @param graphIndex: An index that will be returned in the Signals to be able to use multiple threads at once.
     */
    void setVariables1D(const nix::DataArray &array, nix::NDSize start, nix::NDSize extent, nix::Dimension dim, int graphIndex);

    /**
     * @brief setChuncksize: sets the chunksize of the thread. Defines the size of the parts the thread will split the work.
     * @param size: cannot be 0
     */
    void setChuncksize(unsigned int size);

    void restartThread(nix::NDSize start, nix::NDSize extent);
    void startLoadingIfNeeded(QCPRange range, int xDim, double dataMin, double dataMax, double meanPoints);
    void calcStartExtent(const nix::DataArray &array, nix::NDSize &start_size, nix::NDSize& extent_size, QCPRange curRange, int xDim);
    bool checkForMoreData(const nix::DataArray &array, double currentExtreme, bool higher, int xDim);

private:
    void getAxis(nix::Dimension dim, QVector<double> &axis, unsigned int count, unsigned int offset);
    bool testInput(const nix::DataArray &array, nix::NDSize start, nix::NDSize extent);
    void load1D(nix::DataArray array, nix::NDSize start, nix::NDSize extent, nix::Dimension dim, unsigned int chunksize, int graphIndex);
    void load2D(nix::DataArray array, nix::NDSize start, nix::NDSize extent, nix::Dimension dim,  unsigned int dimNumber, std::vector<int> index2D, unsigned int chunksize, int Index);

signals:
    /**
     * @brief dataReady: Signal that is triggered after the thread finished loading the data.
     * @param data: the data that was loaded.
     * @param axis: the corresponding part of the given x-axis fitting to the part of the data
     * @param index: 1D: the index given at the start, 2D: the index given at the start + the index of the second dimension.
     */
    void dataReady(const QVector<double> &data, const QVector<double> &axis, int index);
    /**
     * @brief progress: Signal triggerd after each chunk a part of the data is loaded.
     * @param percent: number between 0-1. Starts at 0 and ends one step bevor 1.
     * @param index: 1D: the index given at the start, 2D: the index given at the start + the index of the second dimension.
     */
    void progress(double percent, int index);

private:
    QMutex mutex;
    QWaitCondition condition;
    nix::DataArray array;
    nix::NDSize start;
    nix::NDSize extent;
    unsigned int chunksize;
    nix::Dimension dim;
    unsigned int dimNumber;
    std::vector<int> index2D;
    int graphIndex;
    bool abort;
    bool restart;
};

#endif // LOADTHREAD_H
