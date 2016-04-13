#ifndef RECENTFILEOPTIONS_HPP
#define RECENTFILEOPTIONS_HPP

#include <QWidget>

namespace Ui {
class RecentFileOptions;
}

class RecentFileOptions : public QWidget
{
    Q_OBJECT

public:
    explicit RecentFileOptions(QWidget *parent = 0);
    ~RecentFileOptions();

private:
    Ui::RecentFileOptions *ui;
};

#endif // RECENTFILEOPTIONS_HPP
