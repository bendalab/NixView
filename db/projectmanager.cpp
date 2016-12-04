#include "projectmanager.hpp"
#include <QDir>
#include <QFile>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include "projectindex.hpp"

ProjectManager::ProjectManager() {}


ProjectManager::ProjectManager(const QString &path) {
    if (!QSqlDatabase::contains("projects_db")) {
        QSqlDatabase project_db = QSqlDatabase::addDatabase("QSQLITE", "projects_db");
        QFile db(path);
        if (!db.exists()) {
            std::cerr << "database does not exist!"<< std::endl;
            create_new_database(project_db, path);
        } else {
            project_db.setDatabaseName(path);
            if (!project_db.open()) {
                std::cerr << "failed to open database!\n";
            }
        }
        project_db.close();
    }
}


ProjectManager::~ProjectManager() {
    //project_db.close();
}


QSqlQuery ProjectManager::project_list() {
    QSqlDatabase db = QSqlDatabase::database("projects_db");
    db.open();
    QSqlQuery query(db);
    query.prepare("SELECT name FROM projects");
    query.exec();
    QSqlDatabase::database("projects_db").close();
    return query;
}


}


bool ProjectManager::create_new_database(const QString &path) {
    project_db.setDatabaseName(path);
    project_db.open();
    QSqlQuery q(project_db);
    return q.exec(QLatin1String("create table projects(id integer primary key, name varchar, project_index varchar)"));
}


bool ProjectManager::add_project(const QString &path) const {
    bool success = false;
    QDir dir(path);
    QString name = dir.dirName();
    if (!ProjectIndex::create_project_index(path))
            return success;
    if (project_db.isOpen() && !name.isEmpty() && check_project_name(name)) {
        QSqlQuery query(QSqlDatabase::database("projects_db"));
        query.prepare("INSERT INTO projects (name) VALUES (:name)");
        query.bindValue(0, name);
        if(query.exec()) {
            success = true;
        } else {
            std::cerr << "add_project error:  " << query.lastError().text().toStdString();
        }
    }
    QSqlDatabase::database("projects_db").close();
    return success;
}


bool ProjectManager::remove_project(const QString &name) {
    QSqlDatabase::database("projects_db").open();
    QSqlQuery q(QSqlDatabase::database("projects_db"));
    q.prepare("DELETE FROM projects WHERE name = (:name)");
    q.bindValue(0, name);
    bool success = q.exec();
    QSqlDatabase::database("projects_db").open();
    return success;
}


bool ProjectManager::rename_project(const QString &old_name, const QString &new_name) {
    QSqlDatabase::database("projects_db").open();
    QSqlQuery q(QSqlDatabase::database("projects_db"));
    q.prepare("UPDATE projects SET name=(:new_name) WHERE name = (:old_name)");
    q.bindValue(":new_name", new_name);
    q.bindValue(":old_name", old_name);
    bool success = q.exec();
    if (!success) {
        std::cerr << q.lastError().text().toStdString() << std::endl;
    }
    QSqlDatabase::database("projects_db").close();
    return success;
}


bool ProjectManager::check_project_name(const QString &name) const {
    bool is_valid = false;
    QSqlDatabase::database("projects_db").open();
    QSqlQuery query(QSqlDatabase::database("projects_db"));
    query.prepare("SELECT name FROM projects WHERE name = (:name)");
    query.bindValue(0, name);
    if (query.exec()) {
        is_valid = !query.next();
    }
    QSqlDatabase::database("projects_db").close();
    return is_valid;
}
