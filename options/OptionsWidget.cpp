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
    QTabWidget *view_tab_widget = new QTabWidget();

    tree_view_options = new TreeViewOptions();
    file_options = new RecentFileOptions();
    QObject::connect(file_options, SIGNAL(recent_files_update(QStringList)), this, SLOT(recent_file_update(QStringList)));
    view_tab_widget->addTab(tree_view_options, "TreeView");
    view_tab_widget->addTab(file_options, "Recent files");
    ui->verticalLayout->addWidget(view_tab_widget);
}


void OptionsWidget::file_opened(QString filename) {
    file_options->set_file(filename);
}


void OptionsWidget::recent_file_update(QStringList files) {
    emit recent_file_update_signal(files);
}


OptionsWidget::~OptionsWidget()
{
    delete ui;
}
