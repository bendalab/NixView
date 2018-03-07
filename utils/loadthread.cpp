#include "loadthread.h"
#include <QVector>

LoadThread::LoadThread(QObject *parent):
    QThread(parent) {
    abort = false;
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
        mutex.unlock();

        uint dataLength = 0;
        for (uint i=0; i<extend.size(); i++) {
            if(extend[0] > dataLength) // extend has to define 1d data (all 1 exept for one entry)
                dataLength = extend[0];
        }
        loadedData.resize(dataLength);

        array.getData(array.dataType(),loadedData.data(),extend, start);

        emit dataReady(loadedData, start, extend);
        condition.wait(&mutex);

        if(abort) {
            return;
        }
    }
}



bool LoadThread::setVariables(const nix::DataArray &array, nix::NDSize start, nix::NDSize extend) {
    if(! testInput(array, start, extend)) {
        return false;
    }

    mutex.lock();
    this->array = array;
    this->start = start;
    this->extend = extend;
    condition.wakeOne();
    mutex.unlock();
    return true;
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

        if(extend[i] != 1) { // extend has to define 1d data (all 1 exept for one entry)
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
