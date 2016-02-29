#ifndef NIXDATAMODEL_H
#define NIXDATAMODEL_H

#include <QStandardItemModel>
#include <nix.hpp>
#include <model/NixModelItem.hpp>

typedef QList<QStandardItem*> Row;
typedef QList<QString> RowStrings;

class NixDataModel : public QStandardItemModel
{
public:
    NixDataModel(nix::File);
    int num_columns;

private:
    nix::File nix_file;
    QString s_to_q(std::string);
    template<typename T>
    std::string get_created_at(T);
    template<typename T>
    std::string get_updated_at(T);
    void nix_file_to_model();

    template<typename T>
    void add_content(QStandardItem*, T nix_entity);

    void add_subsec_prop(QStandardItem*, nix::Section);

    template<typename T>
    RowStrings create_rowstrings(T arg, std::string storagetype, std::string root_child_link, std::string nixtype="", std::string shape="");

    template<typename T>
    QList<QStandardItem*> create_entry_row(QList<QString>, T nix_entity);

    QList<QStandardItem*> create_entry_row(QList<QString>);

    template<typename T>
    void add_linked_sources(QStandardItem* item, T nix_entity);

    template<typename T>
    void add_linked_metadata(QStandardItem* item, T nix_entity);

    std::string get_property_value(nix::Property);
    void add_tag_info(QStandardItem*, nix::Tag);
    void add_multitag_info(QStandardItem*, nix::MultiTag);
};

#endif // NIXDATAMODEL_H
