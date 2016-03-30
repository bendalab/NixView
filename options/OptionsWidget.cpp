#include "OptionsWidget.hpp"
#include "ui_OptionsWidget.h"
#include <QProxyStyle>
#include "infowidget/DescriptionPanel.hpp"

class HorizontalTabBarStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget) const
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab)
            s.transpose();
        return s;
    }

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel)
        {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
            {
                QStyleOptionTab opt(*tab);
                opt.shape = QTabBar::RoundedNorth;
                QProxyStyle::drawControl(element, &opt, painter, widget);
                return;
            }
        }
        QProxyStyle::drawControl(element, option, painter, widget);
    }
};


OptionsWidget::OptionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OptionsWidget)
{
    ui->setupUi(this);
    setWindowTitle("Properties");

    tw = new OptionsTabWidget();
    tw->tabBar()->setStyle(new HorizontalTabBarStyle);

    // view options
    QTabWidget *view_tab_widget = new QTabWidget();
    tree_view_options = new TreeViewOptions();
    view_tab_widget->addTab(tree_view_options, "TreeView");

    tw->addTab(view_tab_widget, "Views");
//    tw->addTab(new DescriptionPanel(), "Other Stuff");

    ui->verticalLayout->addWidget(tw);
}

OptionsWidget::~OptionsWidget()
{
    delete ui;
}
