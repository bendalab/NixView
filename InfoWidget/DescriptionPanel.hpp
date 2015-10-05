#ifndef DESCRIPTIONPANEL_HPP
#define DESCRIPTIONPANEL_HPP

#include <QWidget>
#include <QVariant>
#include <boost/optional.hpp>
#include <sstream>

namespace Ui {
class DescriptionPanel;
}

class DescriptionPanel : public QWidget
{
    Q_OBJECT

public:
    explicit DescriptionPanel(QWidget *parent = 0);
    ~DescriptionPanel();

public slots:
    void update_description_panel(std::string id, std::string type, std::string name, boost::optional<std::basic_string<char>> description);
    void update_description_panel(std::string id, std::string name, boost::optional<std::basic_string<char>> description);
    void update_description_panel(QVariant v);

private:
    Ui::DescriptionPanel *ui;
};

#endif // DESCRIPTIONPANEL_HPP
