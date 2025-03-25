#include "database/watering_record_helper.hpp"

/*
int WateringRecordHelper::create_record(std::string date_str) {

}

int WateringRecordHelper::del_record(std::string start_date_str, std::string end_date_str) {

}

std::ifstream* WateringRecordHelper::record_read_stream(std::string date_str) {
    std::ifstream* input_stream = new std::ifstream(record_folder + date_str);
    //fail() function can give information of file existance, so no need to explicitly check file existance.
    if(input_stream->fail()) {
        return nullptr;
    } else {
        return input_stream;
    }
}

std::ofstream* WateringRecordHelper::record_write_stream(std::string date_str) {
    std::ofstream* output_stream = new std::ofstream(record_folder + date_str);
    //output_stream.open();
    if(output_stream->fail()) {
        return nullptr;
    } else {
        return output_stream;
    }
}

int WateringRecordHelper::modify_data(std::string date_str, int attribute, std::string attribute_value_str, bool is_incremental) {
    //1. Read the original file
    std::ifstream* input_file = record_read_stream(record_folder + date_str); // 打开文件
    if (input_file == nullptr) {
        std::cerr << "Failed to open the file" << std::endl;
        return -1;
    }

    std::vector<std::string> lines; // 用于存储文件内容的字符串数组
    std::string line;   //lines[0] is times_of_watering, line[1] is amount_of_watering, line[2] is soil_moisture_percentage

    // 按行读取文件内容
    while (std::getline(*input_file, line)) {
        lines.push_back(line);
    }

    input_file->close(); // 关闭文件
    delete input_file;

    //2. Modify the content
    lines[attribute] == attribute_value_str;

    //3. Write the lines to the file
    std::ofstream* output_file = record_write_stream(record_folder + date_str);
    if(output_file == nullptr) {
        return -1;
    }
    for(int i = 0; i < 3; i++) {
        (*output_file) << lines[i] << std::endl;
    }

    output_file->close();
    delete output_file;
}*/

WateringRecordHelper::WateringRecordHelper() {
    m_sqlite_database = new SQLiteDatabase();
    m_sqlite_database->open(WATERING_RECORD_DB_PATH);
}

WateringRecordHelper::~WateringRecordHelper() {
    m_sqlite_database->close();
    delete m_sqlite_database;
}

int WateringRecordHelper::modify_record(std::string date_str, int attribute, std::string attribute_value_str, bool is_incremental) {

}