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

    // setter
    /**
     * @brief Sets the filter to specified mode.
     * @param mode
     * 0 = Validate  with target only, if target invalid, children also invalid
     * 1 = target valid if target or one of subsequent child nodes valid (recursive)
     * 2 = target valid if parent valid
     * 3 = 1 + 2
     */
    void set_filter_mode(int mode) {filter_mode = mode;}

private:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool check_entry_row(int source_row, const QModelIndex &source_parent) const;

};

#endif // NIXPROXYMODEL_H
