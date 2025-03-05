#include <cstdio>
#include <string>
#include <thread>
#include <sqlite3.h>

using namespace std;

static int callback(void *para, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
       printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
 }

int main() {
    //Read the local SQLite database
    //sqlite3_open() will create the database file if it does not exist
    sqlite3 *db;
    int rc = sqlite3_open("server_database.db", &db);
    if(rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        //Database opened successfully
        //Create server_info table if it is the first run
        char* errmsg = 0;

        string createServerInfoSql = "CREATE TABLE IF NOT EXISTS server_info ("
            "server_id TEXT PRIMARY KEY, is_added INTEGER, client_add_time INTEGER, "
            "mode INTEGER, water_amount REAL, automatic_humidity REAL, "
            "scheduled_freq INTEGER, scheduled_time TEXT"
            ");";
        int rc2 = sqlite3_exec(db, createServerInfoSql.c_str(), callback, 0, &errmsg);

        //Read the table server_info
        char* readServerInfoSql = "select * from server_info";
        char** pResult;
        int nRow;
        int nCol;
        int rc3 = sqlite3_get_table(db, readServerInfoSql, &pResult, &nRow, &nCol, nullptr);
        if(rc3 == SQLITE_OK) {
            
            if(nRow >= 2) {
                //If there are two or more rows
                fprintf(stderr, "Server Error: There are more than one records in the table server_info");
            } else if(nRow == 1 && 1) {
                //If there is one record and is_added == 1, then it means that this server is added by a client.
                
            } else if(nRow == 0) {

            } else {
                fprintf(stderr, "Server Error");
            }
        }
    }
    return 0;
}