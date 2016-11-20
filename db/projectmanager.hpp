#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QSqlDatabase>

class QSqlQuery;

class ProjectManager
{
public:
    ProjectManager();
    ProjectManager(const QString &path);
    ~ProjectManager();

    bool check_project_name(const QString &name) const;
    bool add_project(const QString &name) const;
    // bool remove_project(const QString &name);
    // bool rename_project(const QString &name);
    QSqlQuery project_list();
    bool is_open();
    void close();

private:
    QSqlDatabase project_db;

    bool create_new_database(const QString &path);

};

#endif // PROJECTMANAGER_H
