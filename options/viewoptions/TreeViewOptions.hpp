#ifndef VIEWOPTIONS_HPP
#define VIEWOPTIONS_HPP

#include <QWidget>

namespace Ui {
class TreeViewOptions;
}

class TreeViewOptions : public QWidget
{
    Q_OBJECT

public:
    explicit TreeViewOptions(QWidget *parent = 0);
    ~TreeViewOptions();

private:
    Ui::TreeViewOptions *ui;
};

#endif // VIEWOPTIONS_HPP
