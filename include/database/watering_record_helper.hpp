#ifndef WATERINGRECORDHELPER_HPP
#define WATERINGRECORDHELPER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <mutex>
#include "sqlite_database.hpp"

#define WATERING_RECORD_DB_PATH "dbs/watering_record.db"
#define COL_TIMES_OF_WATERING 1
#define COL_AMOUNT_OF_WATERING 2
#define COL_SOIL_MOISTURE_PERCENTAGE 3

class WateringRecordHelper {
private:
    SQLiteDatabase *m_sqlite_database;

    std::mutex water_rec_mtx;

public:
    WateringRecordHelper();
    ~WateringRecordHelper();
    
    int create_table_if_not_exist();
    void create_record_if_not_exist(std::string date_str);
    int update_record(std::string date_str, int col_num, std::string data, bool is_incremental);
    int delete_record(std::string date_str);
    int clear_record();

};

#endif