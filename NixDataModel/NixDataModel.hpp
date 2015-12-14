#ifndef NIXDATAMODEL_H
#define NIXDATAMODEL_H

#include <QStandardItemModel>
#include <nix.hpp>

class NixDataModel : public QStandardItemModel
{
public:
    NixDataModel(nix::File);

private:
    nix::File nix_file;
    QStandardItem* root_node;
    QList<QStandardItem*> create_entry_row(QList<QString>);
    QString s_to_q(std::string);
    template<typename T>
    std::string get_created_at(T);
    template<typename T>
    std::string get_updated_at(T);
    void nix_file_to_model();
};

#endif // NIXDATAMODEL_H
