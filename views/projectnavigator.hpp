#ifndef PROJECTNAVIGATOR_HPP
#define PROJECTNAVIGATOR_HPP

#include <QWidget>
#include <QTreeWidgetItem>
#include <QSqlDatabase>
#include "db/projectmanager.hpp"
#include "db/projectindex.hpp"


namespace Ui {
class ProjectNavigator;
}

class ProjectNavigator : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectNavigator(QWidget *parent = 0);
    ~ProjectNavigator();

    bool set_project(const QString &project);
    void clear();

private:
    Ui::ProjectNavigator *ui;
    ProjectManager pm;
    QString project_name, project_path;
    void open_project(const QString &name, const QString &path);
    // void open_projects_db();

public slots:
    void new_project();
    // void delete_project();
    // void rename_project();
    // void refresh_projects();
    // void select_project(int index);
    void item_selected(QTreeWidgetItem*, int);
    void add_file();
    void remove_file();
    void find();

signals:
    void file_selected(QString);
    void close_file();
};

#endif // PROJECTNAVIGATOR_HPP
