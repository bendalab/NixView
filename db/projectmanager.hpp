#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QSqlDatabase>

class QSqlQuery;
/**
 * @brief The ProjectManager class is supposed to manage all nixview projects. That is, it opens the
 * "projects.db" database, can return a list of projects, you can add and delete projects...
 */
class ProjectManager
{
public:
    ProjectManager();
    ProjectManager(const QString &path);
    ~ProjectManager();

    bool check_project_name(const QString &name) const;
    bool add_project(const QString &path) const;
    bool remove_project(const QString &name);
    bool rename_project(const QString &old_name, const QString &new_name);
    QString get_project_path(const QString &name);
    QSqlQuery project_list();
    QStringList get_project_name_list();

private:
    bool create_new_database(QSqlDatabase db, const QString &path);

};

#endif // PROJECTMANAGER_H
