#ifndef NIXPROXYMODEL_H
#define NIXPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <model/nixtreemodel.h>

class NixProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    NixProxyModel(QObject * parent = 0);

    QStringList fine_filter;
    QString rough_filter;
    bool case_sensitive;
    int filter_mode = 0;
    bool metadata_only_mode = false;
    bool block_mode = false;

    NixTreeModelItem* get_item_from_qml(QModelIndex qml);

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
    bool qml_contains_fine_filter(QModelIndex qml) const;
    bool qml_contains_expression(QModelIndex qml, QString str) const;
    bool entitiy_check(int source_row, const QModelIndex &source_parent, const NixType &type) const;

public slots:
    void set_rough_filter(QString exp);
    void set_fine_filter(QString exp);
    void set_case_sensitivity(bool b);

};

#endif // NIXPROXYMODEL_H
