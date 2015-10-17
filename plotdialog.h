#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H

#include <QDialog>
#include <QVariant>

namespace Ui {
class PlotDialog;
}

class PlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotDialog(QWidget *parent = 0);

    void set_entity(QVariant var);

    ~PlotDialog();

private:
    Ui::PlotDialog *ui;
    QVariant item;

    void draw();
    bool can_draw();

};

#endif // PLOTDIALOG_H
