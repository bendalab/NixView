#ifndef METADATAPANEL_HPP
#define METADATAPANEL_HPP

#include <QWidget>

namespace Ui {
class MetaDataPanel;
}

class MetaDataPanel : public QWidget
{
    Q_OBJECT

public:
    explicit MetaDataPanel(QWidget *parent = 0);
    ~MetaDataPanel();

private:
    Ui::MetaDataPanel *ui;
};

#endif // METADATAPANEL_HPP
