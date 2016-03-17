#ifndef OPTIONSWIDGET_HPP
#define OPTIONSWIDGET_HPP

#include <QWidget>
#include <QTabWidget>
#include "options/OptionsTabWidget.hpp"

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
};

#endif // OPTIONSWIDGET_HPP
