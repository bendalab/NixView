#include "projectmanager.hpp"
#include <QDir>
#include <QFile>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
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


QStringList ProjectManager::get_project_name_list() {
    QSqlDatabase db = QSqlDatabase::database("projects_db");
    db.open();
    QSqlQuery q(db);
    q.prepare("SELECT name FROM projects");
    q.exec();
    QStringList list;
    int index = q.record().indexOf("name");
    while (q.next()) {
        list.append(QStringList(q.value(index).toString()));
    }
    QSqlDatabase::database("projects_db").close();
    return list;
}


bool ProjectManager::create_new_database(QSqlDatabase db, const QString &path) {
    db.setDatabaseName(path);
    db.open();
    QSqlQuery q(db);
    bool success = q.exec(QLatin1String("create table projects(id integer primary key, name varchar, project_index varchar)"));
    db.close();
    return success;
}


bool ProjectManager::add_project(const QString &path) const {
    bool success = false;
    QDir dir(path);
    QString name = dir.dirName();
    if (!ProjectIndex::create_project_index(path))
        return success;
    QSqlDatabase project_db = QSqlDatabase::database("projects_db");
    project_db.open();
    if (project_db.isOpen() && !name.isEmpty() && check_project_name(name)) {
        QSqlQuery query(QSqlDatabase::database("projects_db"));
        query.prepare("INSERT INTO projects (name, project_index) VALUES (:name, :path)");
        query.bindValue(0, name);
        query.bindValue(1, path);
        if(query.exec()) {
            success = true;
        } else {
            std::cerr << "add_project error:  " << query.lastError().text().toStdString();
        }
    }
    project_db.close();
    return success;
}


bool ProjectManager::remove_project(const QString &name) {
    QSqlDatabase::database("projects_db").open();
    QSqlQuery q(QSqlDatabase::database("projects_db"));
    q.prepare("DELETE FROM projects WHERE name = (:name)");
    q.bindValue(0, name);
    bool success = q.exec();
    QSqlDatabase::database("projects_db").close();
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


QString ProjectManager::get_project_path(const QString &name) {
    QString path;
    QSqlDatabase db = QSqlDatabase::database("projects_db");
    db.open();
    QSqlQuery q(db);
    q.prepare("SELECT project_index FROM projects WHERE name = (:name)");
    q.bindValue(":name", name);
    if (q.exec() && q.next()) {
        int fieldNo = q.record().indexOf("project_index");
        path = q.value(fieldNo).toString();
    }
    db.close();
    return path;
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
