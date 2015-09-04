#ifndef ANOTHERTREE_HPP
#define ANOTHERTREE_HPP

#include <QWidget>

namespace Ui {
class AnotherTree;
}

class AnotherTree : public QWidget
{
    Q_OBJECT

public:
    explicit AnotherTree(QWidget *parent = 0);
    ~AnotherTree();

private:
    Ui::AnotherTree *ui;
};

#endif // ANOTHERTREE_HPP
