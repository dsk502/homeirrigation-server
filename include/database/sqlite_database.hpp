#ifndef SQLITEDATABASE_HPP
#define SQLITEDATABASE_HPP

#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>

class SQLiteDatabase {
private:
    sqlite3* m_db;

public:
    //Open database connection
    bool open(const std::string& db_file_path);

    //Close database
    void close();

    //Execute SQL command
    bool exec(const std::string& sql);

    //Query data
    std::vector<std::vector<std::string>> query(const std::string& sql);
};

#endif