#ifndef TAGVIEW_H
#define TAGVIEW_H

#include <QScrollArea>
#include <nix.hpp>
#include <QModelIndex>
#include "model/NixModelItem.hpp"
#include "entitydescriptor.h"

namespace Ui {
class TagView;
}

class TagView : public QScrollArea
{
    Q_OBJECT

public:
    explicit TagView(QWidget *parent = 0);
    ~TagView();

    void setEntity(QModelIndex qml);
    void clear();

public slots:
    void reference_selected();
    void feature_selected();

private:
    Ui::TagView *ui;
    nix::DataArray array;
    nix::Tag tag;
    QModelIndex item_qml;
    NixModelItem* item;

    EntityDescriptor basic_description(const std::string &name, const std::string &type, const std::string &description,
                                       const std::string &id, const std::string &created, const std::string &updated);
    void fill_references();
    void fill_features();
};

#endif // TAGVIEW_H
