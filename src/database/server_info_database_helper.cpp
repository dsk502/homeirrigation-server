#include "database/server_info_database_helper.hpp"

ServerInfoDatabaseHelper::ServerInfoDatabaseHelper() {
    m_sqlite_database = new SQLiteDatabase();
    m_sqlite_database->open(SERVER_INFO_DB_PATH);
}

ServerInfoDatabaseHelper::~ServerInfoDatabaseHelper() {
    m_sqlite_database->close();
    delete m_sqlite_database;
}

//Create the server_info table if not exist
int ServerInfoDatabaseHelper::create_table_if_not_exist() {
    std::string create_server_info_sql = "CREATE TABLE IF NOT EXISTS server_info ("
        "server_id TEXT PRIMARY KEY, is_added INTEGER, client_add_time INTEGER, "
        "mode INTEGER, water_amount REAL, automatic_humidity REAL, "
        "scheduled_freq INTEGER, scheduled_time TEXT"
        ");";
    m_sqlite_database->exec(create_server_info_sql);
}

//Return the number of records
int ServerInfoDatabaseHelper::record_num() {
    std::vector<std::vector<std::string>> records;
    std::string sql_stmt = "SELECT * FROM server_info";
    records = m_sqlite_database->query(sql_stmt);
    return records.size();
}

//Insert a record
int ServerInfoDatabaseHelper::insert_record(std::string client_id, std::string client_pubkey, std::string client_add_time, std::string mode, std::string water_amount, std::string scheduled_freq, std::string scheduled_time) {
    std::string insert_sql = "INSERT INTO server_info (client_id, client_pubkey, client_add_time, mode, water_amount, scheduled_freq, scheduled_time) VALUES ('" + client_id + "', '" + client_pubkey + "', " + client_add_time + ", " + mode + ", " + water_amount + ", " + scheduled_freq + ", '" + scheduled_time + "');";
    bool succeed = m_sqlite_database->exec(insert_sql);
    if(succeed) {
        return 0;
    } else {
        return 1;
    }
}

//Modify the mode info
int ServerInfoDatabaseHelper::update_mode(std::string new_mode, std::string new_water_amount, std::string new_scheduled_freq, std::string new_scheduled_time) {
    if(record_num() == 1) {
        std::string update_sql = "UPDATE server_info SET mode = " + new_mode + ", water_amount = " + new_water_amount + ", scheduled_freq = " + new_scheduled_freq + ", scheduled_time = '" + new_scheduled_time + "';";
        if(m_sqlite_database->exec(update_sql)) {
            return 0;
        } else {
            return 1;
        }
    } else {
        return 1;
    }

}

//Get the server info
struct server_info* ServerInfoDatabaseHelper::get_server_info() {
    server_info* info = new server_info();
    std::vector<std::vector<std::string>> records;
    std::string sql_stmt = "SELECT * FROM server_info";
    records = m_sqlite_database->query(sql_stmt);
    info->client_id = records[0][0];
    info->client_pubkey = records[0][1];
    info->client_add_time = records[0][2];
    info->mode = records[0][3];
    info->water_amount = records[0][4];
    info->scheduled_freq = records[0][5];
    info->scheduled_time = records[0][6];
    return info;
}

//Clear server info
void ServerInfoDatabaseHelper::clear_server_info() {
    std::string clear_server_info_sql = "DELETE FROM server_info;";
    m_sqlite_database->exec(clear_server_info_sql);
}