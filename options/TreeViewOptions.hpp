#ifndef VIEWOPTIONS_HPP
#define VIEWOPTIONS_HPP

#include <QWidget>
#include <QSettings>
#include <QCheckBox>

namespace Ui {
class TreeViewOptions;
}

class TreeViewOptions : public QWidget
{
    Q_OBJECT

public:
    explicit TreeViewOptions(QString role, QWidget *parent = 0);
    ~TreeViewOptions();

public slots:
    void column_state_changed();

signals:
    void column_change(QString role, QString column, bool state);

private:
    QString role;
    Ui::TreeViewOptions *ui;
    QSettings *settings;
    QMap<QCheckBox*, bool> boxes;
    void load_settings();
    void connect_widgets();
    void create_checkboxes();
};

#endif // VIEWOPTIONS_HPP
