#ifndef WATERINGRECORDHELPER_HPP
#define WATERINGRECORDHELPER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include "sqlite_database.hpp"

#define WATERING_RECORD_DB_PATH "bin/dbs/watering_record.db"
#define COL_TIMES_OF_WATERING 1
#define COL_AMOUNT_OF_WATERING 2
#define COL_SOIL_MOISTURE_PERCENTAGE 3

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

#endif