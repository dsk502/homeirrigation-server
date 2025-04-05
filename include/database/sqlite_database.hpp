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
    // 打开数据库连接
    bool open(const std::string& db_file_path);

    // 关闭数据库连接
    void close();

    // 执行SQL语句
    bool exec(const std::string& sql);

    // 查询数据
    std::vector<std::vector<std::string>> query(const std::string& sql);
};

#endif