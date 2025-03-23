#include <cstdio>
#include <string>
#include <thread>
#include <sqlite3.h>
#include "networking/networking_thread.hpp"
#include "crypto/rsa_utils.hpp"
#include "main.hpp"

std::string HomeIrrigationServer::get_raspberry_pi_id() {
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    std::string serial = "0000000000000000"; // 默认值

    if (cpuinfo.is_open()) {
        while (std::getline(cpuinfo, line)) {
            if (line.find("Serial") != std::string::npos) {
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    serial = line.substr(colonPos + 1);
                    serial.erase(serial.begin(), std::find_if(serial.begin(), serial.end(), [](unsigned char ch) {
                        return !std::isspace(ch);
                    }));
                    break;
                }
            }
        }
        cpuinfo.close();
    } else {
        std::cerr << "无法打开 /proc/cpuinfo 文件" << std::endl;
    }

    return serial;
}

int HomeIrrigationServer::server_init() {

    //Init pigpio library
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialise pigpio library" << std::endl;
        return 1;
    }
    return 0;

    //Check if server_id file exists
    //If yes, read the server id; If not, generate the file
    std::ifstream server_id_file_read("server_id.txt");
    if (server_id_file_read) {
        std::cout << "Server id file exist" << std::endl;
        std::getline(server_id_file_read, m_server_id);
        server_id_file_read.close();
    } else {
        std::cout << "Server id file does not exist" << std::endl;
        server_id_file_read.close();
        m_server_id = get_raspberry_pi_id();
        std::ofstream server_id_file_write("server_id.txt");
        server_id_file_write << m_server_id << std::endl;
        server_id_file_write.close();
    }

    //Read server_info database to determine whether this device is added by the client
    m_server_info_database_helper = new ServerInfoDatabaseHelper();
    int num_of_records = m_server_info_database_helper->record_num();
    if(num_of_records == 1) {
        m_is_added == true;
    } else if(num_of_records == 0) {
        m_is_added == false;
    } else {
        //Server Error
        std::cout << "Server Error" << std::endl;
        return 1;
    }

    if(m_is_added) {   //If the device is added
        //Read the keys from key files
        m_server_pubkey = RSAUtils::read_key_from_file(true);
        m_server_prikey = RSAUtils::read_key_from_file(false);
        
    } else {    //If the device is not added
        
    }
    std::thread net_thread(NetworkingThread::networking_thread_main(this));

}

/*
static int callback(void *para, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
       printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}*/

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

        std::string createServerInfoSql = "CREATE TABLE IF NOT EXISTS server_info ("
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
            } else if(nRow == 1 && pResult[nCol + 1] == "1") {
                //If there is one record and is_added == 1, then it means that this server is added by a client.
                //is_added data is in pResult[nCol + 1]
                //Start a thread to listen to the requests from the client
                std::thread server_thread = std::thread(server_func, 1);
                server_thread.join();
            } else if(nRow == 0) {
                //If the server is not added by a client, start a thread to listen the addition request
                std::thread server_thread = std::thread(server_func, 0);
                server_thread.join();
            } else {
                fprintf(stderr, "Server Error");
            }
        }
    }
    return 0;
}