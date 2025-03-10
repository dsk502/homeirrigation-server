#include "headers/sqlite_database_helper.h"

SQLiteDatabaseHelper::SQLiteDatabaseHelper(std::string dbPath) {
    this->dbPath = dbPath.c_str();
}

//Try to open the database and return whether it is successful.
//rc == 0 is successful, otherwise there is a problem
int SQLiteDatabaseHelper::openDb() {
    int result = sqlite3_open(dbPath, &db);
    return result;
}

int SQLiteDatabaseHelper::runSQL(std::string sqlStatement) {
    //int result = sqlite3_exec(db, sqlStatement.c_str(), )
    return -1;
}