#ifndef RECENTFILEOPTIONS_HPP
#define RECENTFILEOPTIONS_HPP

#include <QWidget>
#include <QSettings>

namespace Ui {
class RecentFileOptions;
}

class RecentFileOptions : public QWidget
{
    Q_OBJECT

public:
    explicit RecentFileOptions(QWidget *parent = 0);
    ~RecentFileOptions();

    void set_file(QString filename);

signals:
    void recent_files_update(QStringList);

private:
    Ui::RecentFileOptions *ui;
    QSettings *settings;
    QStringList recent_files;

    void load_settings();
};

#endif // RECENTFILEOPTIONS_HPP
