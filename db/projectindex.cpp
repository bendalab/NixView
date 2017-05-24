#include "projectindex.hpp"
#include <QFile>
#include <QDir>
#include <iostream>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <nix.hpp>
#include "utils/entitydescriptor.h"


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
    if(!q.exec()) {
        db.close();
        return list;
    }
    while (q.next()) {
        list.append(q.value(0).toString());
    }
    db.close();
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
    if(!q.exec()) {
        db.close();
        return file_path;
    }
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
    QSqlQuery q4(db);
    q4.prepare("DELETE FROM metadata_index WHERE metadata_index.entity_id = data_index.id AND data_index.file_id = :id");\

    q4.bindValue(":id", QVariant(pri_key));
    if (!q4.exec()) {
        std::cerr << q4.lastError().text().toStdString() << std::endl;
        std::cerr << "Something went wrong when removing metadata_indexes of file: " << file_path.toStdString() << std::endl;
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


void ProjectIndex::index_block(const nix::Block &block, int f_id) {
    std::cerr << "indexing Block " << block.name() << std::endl;
    QSqlDatabase db = QSqlDatabase::database(this->path);
    db.open();
    std::string def = block.definition() ? *(block.definition()) : "";
    std::string descr = block.name() + " " + block.type() + " " + def;
    QString block_path(block.name().c_str());
    QSqlQuery query(db);
    int block_id = store_data_index(f_id, QString::fromStdString(block.id()), QString::fromStdString(block.type()),
                                    QString::fromStdString(descr), block_path, query);
    if (block.metadata() && block_id > -1) {
        index_metadata(block.metadata(), block_id, query);
    }
    for (nix::DataArray a : block.dataArrays()) {
        index_data_array(a, f_id, query, block_path);
    }
    for(nix::Tag t : block.tags()) {
        index_tag(t, f_id, query, block_path);
    }
    for(nix::MultiTag t : block.multiTags()) {
        index_mtag(t, f_id, query, block_path);
    }
    db.close();
}


void ProjectIndex::index_data_array(const nix::DataArray &array, int file_id, QSqlQuery &query, const QString &parent_path) {
    std::cerr << "indexing dataArray " << array.name() << std::endl;
    std::string def = array.definition() ? *(array.definition()) : "";
    std::string descr = array.name() + " " + array.type() + " " + def;
    int array_id = store_data_index(file_id, QString::fromStdString(array.id()), QString::fromStdString(array.type()),
                                    QString::fromStdString(descr), parent_path + "/" + QString::fromStdString(array.name()), query);
    if (array.metadata() && array_id > -1) {
        index_metadata(array.metadata(), array_id, query);
    }
}


void ProjectIndex::index_tag(const nix::Tag &tag, int file_id, QSqlQuery &query, const QString &parent_path) {
    std::cerr << "indexing Tag " << tag.name() << std::endl;
    std::string def = tag.definition() ? *(tag.definition()) : "";
    std::string descr = tag.name() + " " + tag.type() + " " + def;
    int tag_id = store_data_index(file_id, QString::fromStdString(tag.id()), QString::fromStdString(tag.type()),
                                  QString::fromStdString(descr), parent_path + "/" + QString::fromStdString(tag.name()), query);
    if (tag.metadata() && tag_id > -1) {
        index_metadata(tag.metadata(), tag_id, query);
    }
}


void ProjectIndex::index_mtag(const nix::MultiTag &mtag, int file_id, QSqlQuery &query, const QString &parent_path) {
    std::cerr << "indexing MultiTag " << mtag.name() << std::endl;
    std::string def = mtag.definition() ? *(mtag.definition()) : "";
    std::string descr = mtag.name() + " " + mtag.type() + " " + def;
    int mtag_id = store_data_index(file_id, QString::fromStdString(mtag.id()), QString::fromStdString(mtag.type()),
                                    QString::fromStdString(descr), parent_path + "/" + QString::fromStdString(mtag.name()), query);
    if (mtag.metadata() && mtag_id > -1) {
        index_metadata(mtag.metadata(), mtag_id, query);
    }
}


int ProjectIndex::store_data_index(int file_id, const QString &entity_id, const QString &entity_type,
                      const QString &description, const QString &entity_path, QSqlQuery &query) {
    query.prepare("INSERT INTO data_index (file_id, entity_id, entity_type, entity_description, entity_path) VALUES (:id, :ent_id, :type, :descr, :path)");
    query.bindValue(":id", QVariant(file_id));
    query.bindValue(":ent_id", QVariant(entity_id));
    query.bindValue(":type", QVariant(entity_type));
    query.bindValue(":descr", QVariant(description));
    query.bindValue(":path", QVariant(entity_path));
    query.exec();
    query.prepare("SELECT id FROM data_index WHERE entity_id = :id");
    query.bindValue(":id", QVariant(entity_id));
    int id = (query.exec() && query.next()) ? query.value(query.record().indexOf("id")).toInt() : -1;
    return id;
}


void ProjectIndex::index_metadata(const nix::Section &section, int entity_id, QSqlQuery &query) {
    query.prepare("INSERT INTO metadata_index (data_id, section_id, metadata) VALUES (:entity_id, :sec_id, :metadata)");
    QString sec_name(section.name().c_str());
    QString sec_id(section.id().c_str());
    QString sec_type(section.type().c_str());
    for (nix::Property p : section.properties()) {
        QString metadata = sec_name + " " + sec_type + " " + QString::fromStdString(p.name());
        for (nix::Value v : p.values()) {
            metadata  = metadata + " " + QString::fromStdString(EntityDescriptor::value_to_str(v, p.dataType()));
        }
        query.bindValue(":entity_id", QVariant(entity_id));
        query.bindValue(":sec_id", QVariant(sec_id));
        query.bindValue(":metadata", QVariant(metadata));
        query.exec();
    }
    for (nix::Section s : section.sections()) {
        index_metadata(s, entity_id, query);
    }
}


void ProjectIndex::index_file(const QString &file_path) {
    std::cerr << "indexing file " << file_path.toStdString() << std::endl;
    int f_id = file_id(file_path);
    nix::File file = nix::File::open(file_path.toStdString());
    // walk through data
    for (nix::Block b : file.blocks()) {
        index_block(b, f_id);
    }
    file.close();
    std::cerr << "... finished!" << std::endl;
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
        success = q.exec(QLatin1String("CREATE TABLE files(id integer primary key, name varchar, path varchar)"));
        if (success)
            success = q.exec(QLatin1String("CREATE TABLE data_index(id integer primary key, file_id integer, entity_id varchar, entity_type varchar, entity_path varchar, entity_description varchar)"));
        if (success)
            q.exec(QLatin1String("ALTER TABLE data_index ADD FOREIGN KEY (file_id) REFERENCES files (id)"));
        success = q.exec(QLatin1String("CREATE TABLE metadata_index(id integer primary key, data_id integer, section_id, metadata varchar)"));
        if (success)
            q.exec(QLatin1String("ALTER TABLE metadata_index ADD FOREIGN KEY (data_id) REFERENCES data_index (id)"));

        q.prepare(QString("PRAGMA user_version=%1").arg(1));
        index_db.close();
    }
    return success;
}

