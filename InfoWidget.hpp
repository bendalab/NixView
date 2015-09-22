#ifndef INFOWIDGET_HPP
#define INFOWIDGET_HPP

#include <QWidget>
#include <QtGui>

namespace Ui {
class InfoWidget;
}

class InfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InfoWidget(QWidget *parent = 0);
    ~InfoWidget();

    bool is_active;

public slots:
    void update_info_widget(QTreeWidgetItem*, int);

signals:
    void add_info_widget();

private:
    Ui::InfoWidget *ui;

};

#endif // INFOWIDGET_HPP
