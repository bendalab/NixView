#ifndef OPTIONSWIDGET_HPP
#define OPTIONSWIDGET_HPP

#include <QWidget>
#include <QTabWidget>
#include "options/OptionsTabWidget.hpp"
#include "options/TreeViewOptions.hpp"

namespace Ui {
class OptionsWidget;
}

class OptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OptionsWidget(QWidget *parent = 0);
    ~OptionsWidget();

private:
    Ui::OptionsWidget *ui;
    OptionsTabWidget *tw;
    TreeViewOptions *tree_view_options;

friend class MainWindow;
};

#endif // OPTIONSWIDGET_HPP
