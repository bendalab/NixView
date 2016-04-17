#ifndef NIXARRAYTABLEMODEL_H
#define NIXARRAYTABLEMODEL_H

#include <QAbstractTableModel>
#include <nix.hpp>

class NixArrayTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit NixArrayTableModel(QObject *parent = 0);

    void set_source(const nix::DataArray &array);
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    nix::NDSize shape;
    nix::ndsize_t rows, cols;
    nix::DataArray array;
};

#endif // NIXARRAYTABLEMODEL_H
