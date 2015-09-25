#ifndef INFOWIDGET_HPP
#define INFOWIDGET_HPP

#include <QWidget>
#include <QtGui>
#include <boost/optional.hpp>

namespace Ui {
class InfoWidget;
}

class InfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InfoWidget(QWidget *parent = 0);
    ~InfoWidget();

    bool is_active;

public slots:
    void update_info_widget(std::string, std::string, std::string, boost::optional<std::basic_string<char>>);
    void update_info_widget(std::string, std::string, boost::optional<std::basic_string<char>>);

signals:
    void add_info_widget();

private:
    Ui::InfoWidget *ui;

};

#endif // INFOWIDGET_HPP
