#ifndef TAGCONTAINER_H
#define TAGCONTAINER_H

#include <nix.hpp>
#include "utils/entitydescriptor.h"


class TagContainer {
public:
    TagContainer();
    TagContainer(QVariant entity);

private:
    QVariant entity;

public:
    QVariant getEntity();
    std::string name();
    std::string type();
    std::vector<nix::DataArray> references();
    std::vector<nix::Feature> features();
    QVector<QVector<double>> positions();
    bool hasExtents();
    QVector<QVector<double>> extents();
    std::string description();

};
#endif // TAGCONTAINER_H
