#ifndef NIXTREEMODEL_H
#define NIXTREEMODEL_H

#include <QAbstractItemModel>
#include <nix.hpp>
#include "nixtreemodelitem.h"


class NixTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit NixTreeModel(QObject *parent = 0);

    ~NixTreeModel();

    void set_entity(const nix::File &nixfile);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

private:
    nix::File file;
    NixTreeModelItem *root_item;
    NixTreeModelItem *data_node;
    NixTreeModelItem *metadata_node;

    void fetchL1Blocks(const nix::File &file);
    void fetchL1Sections(const nix::File &file);
    int checkForKids(NixTreeModelItem *item) const;
    void append_groups(const std::vector<nix::Group> &groups, NixTreeModelItem *parent);
    void append_tags(const std::vector<nix::Tag> &tags, NixTreeModelItem *parent);
    void append_multi_tags(const std::vector<nix::MultiTag> &tags, NixTreeModelItem *parent);
    void append_data_arrays(const std::vector<nix::DataArray> &tags, NixTreeModelItem *parent);
    void append_features(const std::vector<nix::Feature> &feats, NixTreeModelItem *parent);
    void append_sections(const std::vector<nix::Section> &sections, NixTreeModelItem *parent);
    void append_properties(const std::vector<nix::Property> &properties, NixTreeModelItem *parent);
    void append_sources(const std::vector<nix::Source> &sources, NixTreeModelItem *parent);
    void fetch_block(const nix::Block &b, NixTreeModelItem *parent);
    void fetch_data_array(const nix::DataArray &da, NixTreeModelItem *parent);
    void fetch_tag(const nix::Tag &t, NixTreeModelItem *parent);
    void fetch_multi_tag(const nix::MultiTag &mt, NixTreeModelItem *parent);
    void fetch_group(const nix::Group &g, NixTreeModelItem *parent);
    void fetch_section(const nix::Section &s, NixTreeModelItem *parent);
    void fetch_source(const nix::Source &s, NixTreeModelItem *parent);
};

#endif // NIXTREEMODEL_H
