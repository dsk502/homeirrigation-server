#include <sqlite3.h>
#include <string>

class SQLiteDatabaseHelper {
private:
    const char* dbPath;
    sqlite3 *db = nullptr;

public:
    SQLiteDatabaseHelper(std::string dbPath);
    int openDb();
    int runSQL(std::string sqlStatement);
};