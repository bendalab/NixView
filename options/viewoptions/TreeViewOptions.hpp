#ifndef VIEWOPTIONS_HPP
#define VIEWOPTIONS_HPP

#include <QWidget>
#include <QSettings>

namespace Ui {
class TreeViewOptions;
}

class TreeViewOptions : public QWidget
{
    Q_OBJECT

public:
    explicit TreeViewOptions(QWidget *parent = 0);
    ~TreeViewOptions();

public slots:
    void set_name_column_hidden(bool b);

private:
    Ui::TreeViewOptions *ui;
    QSettings *settings;
    void load_settings();
    void connect_widgets();
};

#endif // VIEWOPTIONS_HPP
