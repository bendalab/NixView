#ifndef DESCRIPTIONPANEL_HPP
#define DESCRIPTIONPANEL_HPP

#include <QWidget>

namespace Ui {
class DescriptionPanel;
}

class DescriptionPanel : public QWidget
{
    Q_OBJECT

public:
    explicit DescriptionPanel(QWidget *parent = 0);
    ~DescriptionPanel();

private:
    Ui::DescriptionPanel *ui;
};

#endif // DESCRIPTIONPANEL_HPP
