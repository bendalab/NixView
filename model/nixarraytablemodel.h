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
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    nix::NDSize shape;
    std::vector<std::string> h_labels, v_labels;
    int rows, cols;
    nix::DataArray array;

    QVariant get_dimension_label(int section, int role, const nix::Dimension &dim) const;
};

#endif // NIXARRAYTABLEMODEL_H
