#include <iostream>
#include <string>
#include <fstream>
#include "sqlite_database.hpp"

#define WATERING_RECORD_DB_PATH "dbs/watering_record.db"
#define ATTR_TIMES_OF_WATERING 0
#define ATTR_AMOUNT_OF_WATERING 1
#define ATTR_SOIL_MOISTURE_PERCENTAGE 2

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
    int add_record();
    int del_record(int start_date, int end_date);
    int modify_record(std::string date_str, int attribute, std::string attribute_value_str, bool is_incremental);
    std::vector<std::vector<std::string>> read_records(int start_date);
};