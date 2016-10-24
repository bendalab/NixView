#ifndef TABLEDIALOG_HPP
#define TABLEDIALOG_HPP

#include <QDialog>
#include <QModelIndex>
#include <QAbstractButton>


namespace Ui {
class TableDialog;
}

class TableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TableDialog(QWidget *parent = 0);
    ~TableDialog();

    void set_entity(const QVariant var);

public slots:
    void button_clicked(QAbstractButton *button);
    void accept();
private:
    Ui::TableDialog *ui;

};

#endif // TABLEDIALOG_HPP
