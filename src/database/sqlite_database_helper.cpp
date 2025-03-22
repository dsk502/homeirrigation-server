#include "database/sqlite_database_helper.hpp"

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
}