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
    void set_nixtype_column_hidden(bool b);
    void set_storagetype_column_hidden(bool b);
    void set_datatype_column_hidden(bool b);
    void set_shape_column_hidden(bool b);
    void set_id_column_hidden(bool b);
    void set_createdat_column_hidden(bool b);
    void set_updatedat_column_hidden(bool b);
    void set_value_column_hidden(bool b);
    void set_uncertainty_column_hidden(bool b);

signals:
    void emit_rtv_column_display_changed();

private:
    Ui::TreeViewOptions *ui;
    QSettings *settings;
    void load_settings();
    void connect_widgets();
};

#endif // VIEWOPTIONS_HPP
