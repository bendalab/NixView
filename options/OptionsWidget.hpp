#ifndef OPTIONSWIDGET_HPP
#define OPTIONSWIDGET_HPP

#include <QWidget>
#include <QTabWidget>
#include "options/OptionsTabWidget.hpp"
#include "options/TreeViewOptions.hpp"
#include "options/recentfileoptions.hpp"
#include <QSettings>


namespace Ui {
class OptionsWidget;
}

class OptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OptionsWidget(QWidget *parent = 0);
    ~OptionsWidget();

signals:
    void recent_file_update(QStringList files);
    void column_state_changed(QString who, QString column, bool state);

private:
    Ui::OptionsWidget *ui;
    QSettings *settings;
    TreeViewOptions *tree_view_options, *metadata_view_options;
    RecentFileOptions *file_options;

friend class MainWindow;
};

#endif // OPTIONSWIDGET_HPP
