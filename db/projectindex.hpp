#ifndef PROJECTINDEX_H
#define PROJECTINDEX_H
#include <QString>
#include <QSql>

class ProjectIndex {

private:
    QString path;

    void index_file(const QString &file_path);

public:
    ProjectIndex(const QString &path);

    bool add_file(const QString &file_path);

    static bool create_project_index(const QString &path);
};

#endif // PROJECTINDEX_H
