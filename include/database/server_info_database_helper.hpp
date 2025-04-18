#ifndef SERVERINFODATABASEHELPER_HPP
#define SERVERINFODATABASEHELPER_HPP

#include <string>
#include <vector>
#include <mutex>
#include "sqlite_database.hpp"

#define SERVER_INFO_DB_PATH "dbs/server_info.db"

typedef struct server_info {
    std::string client_id;
    std::string client_pubkey;
    std::string client_add_time;
    std::string mode;
    std::string water_amount;
    std::string scheduled_freq;
    std::string scheduled_time;
} server_info;

class ServerInfoDatabaseHelper {
private:
    SQLiteDatabase* m_sqlite_database;
    std::mutex server_info_mtx;
    
public:
    ServerInfoDatabaseHelper();
    ~ServerInfoDatabaseHelper();
    int create_table_if_not_exist();
    int record_num();
    int insert_record(std::string client_id, std::string client_pubkey, std::string client_add_time, std::string mode, std::string water_amount, std::string scheduled_freq, std::string scheduled_time);
    int update_mode(std::string new_mode, std::string new_water_amount, std::string new_scheduled_freq, std::string new_scheduled_time);
    server_info* get_server_info();
    void clear_server_info();
};

#endif
