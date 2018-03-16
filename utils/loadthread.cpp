#include "loadthread.h"
#include <QVector>


LoadThread::LoadThread(QObject *parent, unsigned int chunksize):
    QThread(parent) {
    abort = false;
    this->chunksize = chunksize;
}

LoadThread::~LoadThread() {
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();

    wait();
}


void LoadThread::run() {
    QVector<double> loadedData;

    while(true) {
        mutex.lock();
        nix::DataArray array = this->array;
        nix::NDSize start = this->start;
        nix::NDSize extend = this->extend;
        unsigned int chunksize = this->chunksize;
        mutex.unlock();


        unsigned int dataLength = 1;
        int readDim =0;
        unsigned int offset = start[0];
        for (unsigned int i=0; i<extend.size(); i++) {
            if(extend[i] > 1) // extend has to define 1d data (all entries 1 exept for one)
                dataLength = extend[i];
                readDim = static_cast<int>(i);
                offset = start[i];
        }

        int totalChunks;
        if( dataLength / chunksize == static_cast<double>(dataLength) / chunksize) {
            totalChunks = dataLength / chunksize;
        } else {
            totalChunks = dataLength / chunksize + 1;
        }

        extend[readDim] = chunksize;
        QVector<double> chunkdata(chunksize);
        for (int i=0; i<totalChunks; i++) {

            if(i == totalChunks-1) {
                extend[readDim] = (dataLength - (totalChunks-1) * chunksize);
                chunkdata.resize((dataLength - (totalChunks-1) * chunksize));
            }
            start[readDim] = offset + i * chunksize;
            array.getData(array.dataType(),chunkdata.data(),extend, start);

            loadedData.append(chunkdata);

            emit(progress(static_cast<double>(i)/totalChunks)); //starts with 0 ends with one step below 1
        }

        emit dataReady(loadedData);

        mutex.lock();
        condition.wait(&mutex);
        mutex.unlock();
        if(abort) {
            return;
    }
}
void LoadThread::getAxis(const nix::DataArray &array, QVector<double> &axis, unsigned int count, unsigned int offset, int xDim) {
    nix::Dimension d = array.getDimension(xDim);
    if(d.dimensionType() == nix::DimensionType::Sample) {
        axis = QVector<double>::fromStdVector(d.asSampledDimension().axis(count, offset));
    } else if(d.dimensionType() == nix::DimensionType::Range) {
        axis = QVector<double>::fromStdVector(d.asRangeDimension().axis(count, offset));
    } else {
        axis.resize(count);
        for (unsigned int i=0; i<count; i++) {
            axis[0] = i+offset;
        }
    }
}


void LoadThread::setVariables(const nix::DataArray &array, nix::NDSize start, nix::NDSize extend) {
    if(! testInput(array, start, extend)) {
        std::cerr << "LoadThread::setVariables(): Input not correct." << std::endl;
        return;
    }

    QMutexLocker locker(&mutex); // locks the members and unlocks them when it goes out of scope.

    this->array = array;
    this->start = start;
    this->extend = extend;

    if(! isRunning()) {
        QThread::start(LowPriority);
    } else {
        condition.wakeOne();
    }
}

void LoadThread::setChuncksize(unsigned int size) {
    mutex.lock();
    chunksize = size;
    mutex.unlock();
}


bool LoadThread::testInput(const nix::DataArray &array, nix::NDSize start, nix::NDSize extend) {
    nix::NDSize size = array.dataExtent();
    if( ! (size.size() == start.size() && size.size() == extend.size())) {
        std::cerr << "DataThread::testInput(): start and/or extend don't have the same dimensionality as the array." << std::endl;
        return false;
    }

    bool Dataload1d = false;
    for(uint i=0; i<size.size(); i++) {
        if(size[i] < start[i]+extend[i]) {
            std::cerr << "DataThread::testInput(): start + extend bigger than length of array in dimension " << i << std::endl;
            return false;
        }

        if(extend[i] > 1) { // extend has to define 1d data (all 1 exept for one entry)
            if (Dataload1d) {
                std::cerr << "DataThread::testInput(): extend defines data that is more than one dimensional." << std::endl;
                return false;
            } else {
                Dataload1d = true;
            }
        }
    }
    if(! Dataload1d) {
        std::cerr << "DataThread::testInput(): using DataThread to load a single datum." << std::endl;
    }

    return true;
}
