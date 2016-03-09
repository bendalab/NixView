#ifndef NIXPROXYMODEL_H
#define NIXPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <model/NixDataModel.hpp>

class NixProxyModel : public QSortFilterProxyModel
{
public:
    NixProxyModel(QObject * parent = 0);

    std::string current_keyword;
    int filter_mode = 0;
    bool metadata_only_mode = false;
    bool block_mode = false;

    NixModelItem* get_item_from_qml(QModelIndex qml);

    // setter
    /**
     * @brief Sets the filter to specified mode.
     * @param mode
     * 0 = Validate  with target only, if target invalid, children also invalid
     * 1 = target valid if target or one of subsequent child nodes valid (recursive)
     * 2 = target valid if parent valid
     * 3 = 1 + 2
     */
    void set_filter_mode(int mode) { filter_mode = mode; }
    void set_metadata_only_mode(bool b) { metadata_only_mode = b; }
    void set_block_mode(bool b) { block_mode = b; }

    void refresh();

private:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool check_parent(const QModelIndex &source_parent) const;
    bool check_children(int source_row, const QModelIndex &source_parent) const;
    bool check_entry_row(int source_row, const QModelIndex &source_parent) const;
    bool check_if_in_data_branch(int source_row, const QModelIndex &source_parent) const;

};

#endif // NIXPROXYMODEL_H
