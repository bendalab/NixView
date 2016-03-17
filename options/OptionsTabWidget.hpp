#ifndef OPTIONSTABWIDGET_H
#define OPTIONSTABWIDGET_H

#include <QTabWidget>

class OptionsTabWidget : QTabWidget
{
    Q_OBJECT

public:
    OptionsTabWidget();

friend class OptionsWidget;
};

#endif // OPTIONSTABWIDGET_H
