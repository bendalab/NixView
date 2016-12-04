#include "projectindex.hpp"
#include <QFile>
#include <QDir>
#include <iostream>
#include <QSqlQuery>
#include <QVariant>

ProjectIndex::ProjectIndex(const QString &path)
    : path(path) {
    QFile f(path);
    if (!f.exists()) {
        create_project_index(path);
    } else {
        if (!QSqlDatabase::contains(path)) {
            QSqlDatabase index_db = QSqlDatabase::addDatabase("QSQLITE",  path);
            index_db.setDatabaseName(path);
            if (!index_db.open()) {
                std::cerr << "failed to open database!\n";
            } else
                std::cerr << "successfully opened database for project:  " << path.toStdString() << std::endl;
            index_db.close();
        }
    }
}


bool ProjectIndex::add_file(const QString &file_path) {
    QFile f(file_path);
    QDir d(file_path);
    QString name = d.dirName();
    if (!f.exists()) {
        return false;
    }
    QSqlDatabase db = QSqlDatabase::database(path);
    db.open();
    QSqlQuery q(db);
    q.prepare("INSERT INTO files (name, path) VALUES (:name, :path)");
    q.bindValue(":name", QVariant(name));
    q.bindValue(":path", QVariant(file_path));
    db.close();
    index_file(file_path);
    return true;
}


void ProjectIndex::index_file(const QString &file_path) {
    QSqlDatabase db = QSqlDatabase::database(this->path);
    db.open();



    db.close();

}


bool ProjectIndex::create_project_index(const QString &path) {
    bool success = false;
    QFile f(path);
    if (f.exists()) {
        success = true;
    } else {
        std::cerr << "create project index" << std::endl;
        QSqlDatabase index_db = QSqlDatabase::addDatabase("QSQLITE",  path);
        index_db.setDatabaseName(path);
        if (!index_db.open()) {
            std::cerr << "failed to open database!\n";
        }
        QSqlQuery q(QSqlDatabase::database(path));
        success = q.exec(QLatin1String("create table files(id integer primary key, name varchar, path varchar)"));
        if(success)
            success = q.exec(QLatin1String("create table data_index(id integer primary key, file_id integer, entity_name varchar, entity_type varchar)"));
        if(success)
            q.exec(QLatin1String("ALTER TABLE data_index ADD FOREIGN KEY (file_id) REFERENCES files (id)"));
        index_db.close();
    }

    return success;
}

