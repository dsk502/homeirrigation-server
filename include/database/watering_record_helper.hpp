#include <iostream>
#include <string>
#include <fstream>
#include "sqlite_database.hpp"

#define WATERING_RECORD_DB_PATH "dbs/watering_record.db"
#define COL_TIMES_OF_WATERING 1
#define COL_AMOUNT_OF_WATERING 2
#define COL_SOIL_MOISTURE_PERCENTAGE 3

/*
class WateringRecordHelper {
    std::string record_folder = "record/";

    //Add Record (Create file)
    int create_record(std::string date_str);

    //Delete record in [start_date_str, end_date_str)
    //Format of date: YYYYMMDD
    int del_record(std::string start_date_str, std::string end_date_str);

    //Get record file input (read) stream
    std::ifstream* record_read_stream(std::string date_str);

    //Get record file output (write) stream
    std::ofstream* WateringRecordHelper::record_write_stream(std::string date_str);

    //Modify one data in a file
    int modify_data(std::string date_str, int attribute, std::string data, bool is_incremental);
};*/

class WateringRecordHelper {
private:
    SQLiteDatabase *m_sqlite_database;

public:
    WateringRecordHelper();
    ~WateringRecordHelper();
    //int add_record();
    
    int create_table_if_not_exist();
    void create_record_if_not_exist(std::string date_str);
    int update_record(std::string date_str, int col_num, std::string data, bool is_incremental);
    int delete_record(std::string date_str);
    int clear_record();
    //std::vector<std::vector<std::string>> read_records(int start_date);

};