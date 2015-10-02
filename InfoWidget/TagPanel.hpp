#ifndef TAGPANEL_HPP
#define TAGPANEL_HPP

#include <QWidget>

namespace Ui {
class TagPanel;
}

class TagPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TagPanel(QWidget *parent = 0);
    ~TagPanel();

public slots:
    void update_tag_panel();

private:
    Ui::TagPanel *ui;
};

#endif // TAGPANEL_HPP
