#include <sqlite3.h>
#include <string>

class SQLiteDatabaseHelper {
private:
    std::string m_db_path;
    sqlite3* m_db = nullptr;

public:
    SQLiteDatabaseHelper(std::string db_ath);
    int open_db();
    int run_SQL(std::string sql_statement);
};