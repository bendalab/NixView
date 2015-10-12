#ifndef TAGPANEL_HPP
#define TAGPANEL_HPP

#include <QWidget>
#include <QVariant>
#include <nix.hpp>

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
    void update_tag_panel(QVariant v);

private:
    Ui::TagPanel *ui;
    void clear_tag_panel();
    std::string extract_tag_info(nix::Tag);
};

#endif // TAGPANEL_HPP
