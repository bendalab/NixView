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

public slots:
    void file_opened(QString filename);
    void recent_file_update(QStringList files);

signals:
    void recent_file_update_signal(QStringList files);

private:
    Ui::OptionsWidget *ui;
    QSettings *settings;
    TreeViewOptions *tree_view_options;
    RecentFileOptions *file_options;

friend class MainWindow;
};

#endif // OPTIONSWIDGET_HPP
