#include "database/sqlite_database.hpp"

/*
SQLiteDatabaseHelper::SQLiteDatabaseHelper(std::string db_path) {
    m_db_path = db_path;
}

//Try to open the database and return whether it is successful.
//rc == 0 is successful, otherwise there is a problem
int SQLiteDatabaseHelper::open_db() {
    int result = sqlite3_open(m_db_path.c_str(), &m_db);
    return result;
}

int SQLiteDatabaseHelper::run_SQL(std::string sql_statement) {
    //int result = sqlite3_exec(db, sqlStatement.c_str(), )
    return -1;
}*/

bool SQLiteDatabase::open(const std::string& db_file_path) {
    if (sqlite3_open(db_file_path.c_str(), &m_db) != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }
    return true;
}

// 关闭数据库连接
void SQLiteDatabase::close() {
    sqlite3_close(m_db);
}

// 执行SQL语句
bool SQLiteDatabase::exec(const std::string& sql) {
    char* err_msg = nullptr;
    int rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    return true;
}

// 查询数据
std::vector<std::vector<std::string>> SQLiteDatabase::query(const std::string& sql) {
    std::vector<std::vector<std::string>> results;
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare SQL statement: " << sqlite3_errmsg(m_db) << std::endl;
        return results;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::vector<std::string> row;
        for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
            row.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i))));
        }
        results.push_back(row);
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute SQL statement: " << sqlite3_errmsg(m_db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return results;
}