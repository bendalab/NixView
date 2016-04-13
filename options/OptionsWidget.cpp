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
    QWidget(parent), ui(new Ui::OptionsWidget) {
    ui->setupUi(this);
    setWindowTitle("Properties");
    tw = new OptionsTabWidget();
    tw->tabBar()->setStyle(new HorizontalTabBarStyle);

    QTabWidget *view_tab_widget = new QTabWidget();
    QTabWidget *files_view_tab = new QTabWidget();

    tree_view_options = new TreeViewOptions();
    file_options = new RecentFileOptions();
    view_tab_widget->addTab(tree_view_options, "TreeView");
    files_view_tab->addTab(file_options, "Recent files");
    tw->addTab(view_tab_widget, "Views");
    tw->addTab(files_view_tab, "Files");
    //tw->addTab(new DescriptionPanel(), "Other Stuff");

    ui->verticalLayout->addWidget(tw);
}

OptionsWidget::~OptionsWidget()
{
    delete ui;
}
