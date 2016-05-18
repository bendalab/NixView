#ifndef METADATAPANEL_HPP
#define METADATAPANEL_HPP

#include <QWidget>
#include <nix.hpp>
#include <QtGui>
#include <QTreeWidget>
#include "model/nixtreemodel.h"
#include "filter/NixProxyModel.hpp"

namespace Ui {
class MetaDataPanel;
}

class MetaDataPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MetaDataPanel(QWidget *parent = 0);
    ~MetaDataPanel();

    void setDataModel(NixTreeModel *_nix_model);
public slots:
    void resizeToContent(QModelIndex);
    void setColumnState(QString column, bool visible);
    void contextMenuRequest(QPoint pos);

public:
    QTreeView* getTreeView();
    void updateMetadataPanel(QModelIndex qml);
    void clearMetadataPanel();

private:
    NixProxyModel* proxy_model;
    void setProxyModel();
    void setColumns();

    Ui::MetaDataPanel *ui;
};

#endif // METADATAPANEL_HPP
