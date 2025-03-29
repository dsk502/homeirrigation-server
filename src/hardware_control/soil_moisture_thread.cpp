#include "hardware_control/soil_moisture_thread.hpp"

SoilMoistureThread::SoilMoistureThread(ADCHardware* adc_hardware, WateringRecordHelper* watering_record_helper) {
    adc_hardware_ptr = adc_hardware;
    watering_record_helper_ptr = watering_record_helper;
}

int SoilMoistureThread::soil_moisture_thread_main() {
    
    while(true) {
        //Calculate the timepoint of the next noon
        //Get current timepoint
        auto start_timepoint = std::chrono::system_clock::now();

        //Convert timepoint to std::time_t
        std::time_t start_time_t = std::chrono::system_clock::to_time_t(start_timepoint);

        //Create a std::tm object
        std::tm start_tm;
        std::memset(&start_tm, 0, sizeof(start_tm)); // 初始化为0

        //Use thread-safe localtime_r
        localtime_r(&start_time_t, &start_tm);

        //If passed 11:50, then tomorrow noon; otherwise today noon
        if(start_tm.tm_hour >= 12 || (start_tm.tm_hour == 11 && start_tm.tm_min >= 50)) {
            //Tomorrow
            start_tm.tm_mday += 1;
            mktime(&start_tm);
        }

        //Set time to 12:00
        start_tm.tm_hour = 12;
        start_tm.tm_min = 0;
        start_tm.tm_sec = 0;

        //Convert back
        std::time_t next_noon_time_t = mktime(&start_tm);
        auto next_noon = std::chrono::system_clock::from_time_t(next_noon_time_t);

        while(true) {
            auto now = std::chrono::system_clock::now();
            if(next_noon - now <= 0) {
                double soil_humidity_percentage = adc_hardware_ptr->read_soil_humidity();
                char buffer[80];
                strftime(buffer, sizeof(buffer), "%Y%m%d", &start_tm);    //Format to YYYYMMDD
                std::string date_str(buffer);
                watering_record_helper_ptr->update_record(date_str, COL_SOIL_MOISTURE_PERCENTAGE, std::to_string(soil_humidity_percentage), false);
                break;
            } else if(stop_thread) {
                return 0;
            } else {
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }
        
        //std::cout << "Tomorrow at noon: " << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S") << std::endl;

    }
    return 0;

}