#include "projectindex.hpp"
#include <QFile>
#include <QDir>
#include <iostream>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
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
            } else {
                std::cerr << "successfully opened database for project:  " << path.toStdString() << std::endl;
                if (version() == 0) {
                    version(1);
                }
            }
            index_db.close();
        }
    }
}


void ProjectIndex::version(int v) {
    QSqlDatabase db = QSqlDatabase::database(path);
    if (!db.open())
        return;
    QSqlQuery q(db);
    q.prepare(QString("PRAGMA user_version=%1").arg(v));
    if(!q.exec())
        std::cerr << "[Error] ProjectIndex::version(int): " << q.lastError().text().toStdString() << std::endl;
    db.close();
}


int ProjectIndex::version() {
    int version = -1;
    QSqlDatabase db = QSqlDatabase::database(path);
    if (!db.open())
        return version;
    QSqlQuery q(db);
    q.prepare(QString("PRAGMA %1").arg("user_version"));
    if(!q.exec()) {
        std::cerr << "[Error] ProjectIndex::version(): " << q.lastError().text().toStdString() << std::endl;
        db.close();
        return version;
    }
    if(q.next()) {
        QVariant value = q.value(0);
        version = value.toInt();
    }
    db.close();
    return version;
}


QStringList ProjectIndex::get_file_list() {
    QStringList list;
    QSqlDatabase db = QSqlDatabase::database(path);
    if (!db.open())
        return list;
    QSqlQuery q(db);
    q.prepare("SELECT name FROM files");
    if(!q.exec())
        return list;
    while (q.next()) {
        list.append(q.value(0).toString());
    }
    return list;
}


QString ProjectIndex::get_file_path(const QString &file_name) {
    QString file_path;
    QSqlDatabase db = QSqlDatabase::database(path);
    if (!db.open())
        return file_path;
    QSqlQuery q(db);
    q.prepare("SELECT path FROM files WHERE name = (:name)");
    q.bindValue(":name", file_name);
    if(!q.exec())
        return file_path;
    if (q.next()) {
        return file_path = q.value(0).toString();
    }
    return file_path;
}


bool ProjectIndex::add_file(const QString &file_path) {
    QFile f(file_path);
    QDir d(file_path);
    QString name = d.dirName();
    if (!f.exists()) {
        return false;
    }
    QSqlDatabase db = QSqlDatabase::database(path);
    if (!db.isOpen())
        db.open();
    QSqlQuery q(db);
    q.prepare("INSERT INTO files (name, path) VALUES (:name, :path)");
    q.bindValue(":name", QVariant(name));
    q.bindValue(":path", QVariant(file_path));
    if (!q.exec()){
        std::cerr << "Something went wrong when adding file: " << file_path.toStdString() << std::endl;
    }
    db.close();
    index_file(file_path);
    return true;
}

int ProjectIndex::file_id(const QString &file_name) {
    QSqlDatabase db = QSqlDatabase::database(path);
    if (!db.isOpen())
        db.open();
    int pri_key = -1;
    QSqlQuery q(db);
    q.prepare("SELECT id FROM files WHERE path = (:name)");
    q.bindValue(":name", file_name);
    if (q.exec() && q.next()) {
        int fieldNo = q.record().indexOf("id");
        QVariant id = q.value(fieldNo);
        pri_key = id.toInt();
    }
    return pri_key;
}

bool ProjectIndex::remove_file(const QString &file_path) {
    QSqlDatabase db = QSqlDatabase::database(path);
    if (!db.isOpen())
        db.open();
    int pri_key = -1;
    QSqlQuery q(db);
    q.prepare("SELECT id FROM files WHERE path = (:name)");
    q.bindValue(":name", file_path);
    if (q.exec() && q.next()) {
        int fieldNo = q.record().indexOf("id");
        QVariant id = q.value(fieldNo);
        pri_key = id.toInt();
    }
    if (pri_key == -1) {
        return false;
    }

    QSqlQuery q2(db);
    q2.prepare("DELETE FROM data_index WHERE file_id = (:id)");
    q2.bindValue(":id", QVariant(pri_key));
    if (!q2.exec()) {
        std::cerr << q2.lastError().text().toStdString() << std::endl;
        std::cerr << "Something went wrong when removing indexes of file: " << file_path.toStdString() << std::endl;
        return false;
    }

    QSqlQuery q3(db);
    q3.prepare("DELETE FROM files WHERE id = (:id)");
    q3.bindValue(":id", QVariant(pri_key));
    if (!q3.exec()){
        std::cerr << q3.lastError().text().toStdString() << std::endl;
        std::cerr << "Something went wrong when removing file: " << file_path.toStdString() << std::endl;
        return false;
    }
    db.close();
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

