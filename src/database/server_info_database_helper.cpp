#include "database/server_info_database_helper.hpp"

ServerInfoDatabaseHelper::ServerInfoDatabaseHelper() {
    m_sqlite_database = new SQLiteDatabase();
    m_sqlite_database->open(SERVER_INFO_DB_PATH);
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