#ifndef PROJECTINDEX_H
#define PROJECTINDEX_H
#include <QString>
#include <QtSql>
/**
 * @brief The ProjectIndex class manages the files within a project.
 * ProjectIndex database has (at the moment) two tables that manage the files.
 * 1. files merely stores file name and path in two separate columns (so far absolute paths...)
 * 2. data_index stores all the different NIX entity_names, ids, entity_types, and path.
 * 3. not implemented yet: a entity_index, basically a triplestore associated with each entity....
 */
namespace nix {
    class Block;
    class Section;
    class DataArray;
    class Tag;
    class MultiTag;
}

class ProjectIndex {

private:
    QString path;

    void index_file(const QString &file_path);
    void version(int);
    void index_block(const nix::Block &block, int file_id);
    void index_metadata(const nix::Section &section, int entity_id, QSqlQuery &query);
    void index_data_array(const nix::DataArray &array, int file_id, QSqlQuery &query, const QString &parent_path);
    void index_tag(const nix::Tag &tag, int file_id, QSqlQuery &query, const QString &parent_path);
    void index_mtag(const nix::MultiTag &mtag, int file_id, QSqlQuery &query, const QString &parent_path);

    int store_data_index(int file_id, const QString &entity_id, const QString &entity_type,
                          const QString &description, const QString &entity_path, QSqlQuery &query);

public:
    ProjectIndex(const QString &path);

    /**
     * @brief version get the version of the project index
     * @return int the version (-1) if not set
     */
    int version();

    /**
     * @brief get_file_list
     * @return QStringList with the files associated with this project
     */
    QStringList get_file_list();

    /**
     * @brief get_file_path
     * @param file_name the filename
     * @return
     */
    QString get_file_path(const QString &file_name);

    /**
     * @brief add_file to project index.
     * @param file_path The full path to the file
     * @return false if operation was not successful, e.g. the file does not exsist.
     */
    bool add_file(const QString &file_path);

    /**
     * @brief remove_file from the project index.
     * @param file_name the name of the file (not its path)
     * @return true if operation was successful
     */
    bool remove_file(const QString &file_name);

    /**
     * @brief file_id returns the id returns the id of the file
     * @param file_name QString the file name
     * @return int the id -1 if not found
     */

    int file_id(const QString &file_name);
    /**
     * @brief create_project_index
     * @param path
     * @return
     * TODO needs to check whether an existing file actually represents a project_index
     */
    static bool create_project_index(const QString &path);


};

#endif // PROJECTINDEX_H
