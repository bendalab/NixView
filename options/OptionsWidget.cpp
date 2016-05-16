#include "OptionsWidget.hpp"
#include "ui_OptionsWidget.h"
#include <QProxyStyle>
#include "infowidget/DescriptionPanel.hpp"
#include "common/Common.hpp"

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
    setWindowTitle("Settings");
    QTabWidget *view_tab_widget = new QTabWidget();
    tree_view_options = new TreeViewOptions(MAIN_TREE_VIEW);
    metadata_view_options = new TreeViewOptions(METADATA_TREE_VIEW);
    file_options = new RecentFileOptions();
    QObject::connect(file_options, SIGNAL(recent_files_update(QStringList)), this, SIGNAL(recent_file_update(QStringList)));
    QObject::connect(tree_view_options, SIGNAL(column_change(QString, QString, bool)), this, SIGNAL(column_state_changed(QString, QString, bool)));
    QObject::connect(metadata_view_options, SIGNAL(column_change(QString, QString, bool)), this, SIGNAL(column_state_changed(QString, QString, bool)));
    view_tab_widget->addTab(tree_view_options, "TreeView");
    view_tab_widget->addTab(metadata_view_options, "Metadata Tree");
    view_tab_widget->addTab(file_options, "Recent files");
    ui->verticalLayout->addWidget(view_tab_widget);
}


OptionsWidget::~OptionsWidget()
{
    delete ui;
}
