#ifndef PROJECTNAVIGATOR_HPP
#define PROJECTNAVIGATOR_HPP

#include <QWidget>

namespace Ui {
class ProjectNavigator;
}

class ProjectNavigator : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectNavigator(QWidget *parent = 0);
    ~ProjectNavigator();

private:
    Ui::ProjectNavigator *ui;
};

#endif // PROJECTNAVIGATOR_HPP
