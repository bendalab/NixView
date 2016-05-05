#ifndef LAZYLOADVIEW_H
#define LAZYLOADVIEW_H

#include <QWidget>
#include <QTreeView>


namespace Ui {
class LazyLoadView;
}

class LazyLoadView : public QWidget
{
    Q_OBJECT

public:
    explicit LazyLoadView(QWidget *parent = 0);
    ~LazyLoadView();

    QTreeView* getTreeView();

public slots:
    void set_column_state(QString column, bool visible);

private:
    Ui::LazyLoadView *ui;
};

#endif // LAZYLOADVIEW_H
