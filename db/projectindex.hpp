#ifndef PROJECTINDEX_H
#define PROJECTINDEX_H
#include <QString>
#include <QtSql>

class ProjectIndex {

private:
    QString path;

    void index_file(const QString &file_path);

public:
    ProjectIndex(const QString &path);

    QStringList get_file_list();
    QString get_file_path(const QString &file_name);
    bool add_file(const QString &file_path);

    static bool create_project_index(const QString &path);
};

#endif // PROJECTINDEX_H
