#include "database/watering_record_helper.hpp"

WateringRecordHelper::WateringRecordHelper() {
    m_sqlite_database = new SQLiteDatabase();
    m_sqlite_database->open(WATERING_RECORD_DB_PATH);
}

WateringRecordHelper::~WateringRecordHelper() {
    m_sqlite_database->close();
    delete m_sqlite_database;
}

int WateringRecordHelper::create_table_if_not_exist() {
    std::string create_table_sql = "CREATE TABLE IF NOT EXISTS watering_record (day INTEGER PRIMARY KEY, times_of_watering INTEGER, amount_of_watering REAL, soil_moisture_percentage REAL);";
    m_sqlite_database->exec(create_table_sql);
    return 0;
}

//Update a record in watering_record table
int WateringRecordHelper::update_record(std::string date_str, int col_num, std::string data, bool is_incremental) {
    create_record_if_not_exist(date_str);
    std::string data_to_write;
    if(is_incremental) {
        std::string query_sql = "SELECT * FROM watering_record where day = " + date_str + ";";
        auto record = m_sqlite_database->query(query_sql);
        std::string old_data = record[0][col_num];
        if(col_num == COL_TIMES_OF_WATERING) {
            data_to_write = std::to_string(std::stoi(old_data) + std::stoi(data));
        } else {
            data_to_write = std::to_string(std::stod(old_data) + std::stod(data));
        }
    } else {
        data_to_write = data;
    }
    std::string col_name;
    if(col_num == COL_TIMES_OF_WATERING) {
        col_name = "times_of_watering";
    } else if (col_num == COL_AMOUNT_OF_WATERING) {
        col_name = "amount_of_watering";
    } else {
        col_name = "soil_moisture_percentage";
    }
    //std::string write_sql = "INSERT OR REPLACE INTO watering_record (day, times_of_watering, amount_of_watering, soil_moisture_percentage) VALUES"
    std::string write_sql = "UPDATE watering_record SET " + col_name + " = " + data_to_write + " where day = " + date_str + ";"; 
    m_sqlite_database->exec(write_sql);
    return 0;
}

void WateringRecordHelper::create_record_if_not_exist(std::string date_str) {
    std::string create_record_sql = "INSERT OR IGNORE INTO watering_record (day, times_of_watering, amount_of_watering, soil_moisture_percentage) VALUES (" + date_str + ", 0, 0, 0);";
    m_sqlite_database->exec(create_record_sql);
}

int WateringRecordHelper::delete_record(std::string date_str) {
    std::string delete_record_sql = "DELETE FROM watering_record WHERE day = " + date_str + ";";
    m_sqlite_database->exec(delete_record_sql);
    return 0;
}

int WateringRecordHelper::clear_record() {
    std::string clear_record_sql = "DELETE FROM watering_record;";
    m_sqlite_database->exec(clear_record_sql);
    return 0;
}

