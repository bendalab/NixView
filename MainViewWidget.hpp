#ifndef MAINVIEWWIDGET_HPP
#define MAINVIEWWIDGET_HPP

#include <QWidget>

namespace Ui {
class MainViewWidget;
}

class MainViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainViewWidget(std::string& nix_file_path, QWidget *parent = 0);
    ~MainViewWidget();

private:
    Ui::MainViewWidget *ui;
};

#endif // MAINVIEWWIDGET_HPP
