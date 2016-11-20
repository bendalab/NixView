#include "projectmanager.hpp"
#include <QDir>
#include <QFile>
#include <iostream>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

ProjectManager::ProjectManager() {}


ProjectManager::ProjectManager(const QString &path) {
    project_db = QSqlDatabase::addDatabase("QSQLITE");
    QFile db(path);
    if (!db.exists()) {
        std::cerr << "database does not exist!"<< std::endl;
        create_new_database(path);
    } else {
        project_db.setDatabaseName(path);
        if (!project_db.open()) {
            std::cerr << "failed to open database!\n";
        }
    }
}


ProjectManager::~ProjectManager() {
    //project_db.close();
}


QSqlQuery ProjectManager::project_list() {
    QSqlQuery query(project_db);
    if (project_db.isOpen()) {
        query.prepare("SELECT name FROM projects");
        query.exec();
    } else
        std::cerr << "db not open!!!" << std::endl;

    return query;
}


bool ProjectManager::is_open() {
    return project_db.isOpen();
}


void ProjectManager::close() {
    if (project_db.isOpen())
        project_db.close();
}


bool ProjectManager::create_new_database(const QString &path) {
    project_db.setDatabaseName(path);
    project_db.open();
    QSqlQuery q(project_db);
    return q.exec(QLatin1String("create table projects(id integer primary key, name varchar, project_index varchar)"));
}


bool ProjectManager::add_project(const QString &name) const {
    bool success = false;
    if (project_db.isOpen() && !name.isEmpty() && check_project_name(name)) {
        QSqlQuery query;
        query.prepare("INSERT INTO projects (name) VALUES (:name)");
        query.bindValue(0, name);
        if(query.exec()) {
            success = true;
        } else {
            std::cerr << "add_project error:  " << query.lastError().text().toStdString();
        }
    }
    return success;
}


bool ProjectManager::remove_project(const QString &name) {
    if (project_db.isOpen()) {
        QSqlQuery q(project_db);
        q.prepare("DELETE FROM projects WHERE name = (:name)");
        q.bindValue(0, name);
        return q.exec();
    }
    return false;
}


bool ProjectManager::check_project_name(const QString &name) const {
    bool is_valid = false;
    QSqlQuery query;
    query.prepare("SELECT name FROM projects WHERE name = (:name)");
    query.bindValue(0, name);
    if (query.exec()) {
        is_valid = !query.next();
    }
    return is_valid;
}
