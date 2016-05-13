#ifndef RECENTFILEOPTIONS_HPP
#define RECENTFILEOPTIONS_HPP

#include <QWidget>
#include <QSettings>
#include <QListWidgetItem>


namespace Ui {
class RecentFileOptions;
}

class RecentFileOptions : public QWidget
{
    Q_OBJECT

public:
    explicit RecentFileOptions(QWidget *parent = 0);
    ~RecentFileOptions();

signals:
    void recent_files_update(QStringList);

public slots:
    void item_selected(QListWidgetItem *current, QListWidgetItem *past);
    void move_item_up();
    void move_item_down();
    void delete_item();
    void file_count_update();

private:
    Ui::RecentFileOptions *ui;
    QStringList recent_files;

    void load_settings();
    void fill_list();
};

#endif // RECENTFILEOPTIONS_HPP
