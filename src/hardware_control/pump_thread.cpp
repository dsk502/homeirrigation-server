#include "hardware_control/pump_thread.hpp"

PumpThread::PumpThread(WateringRecordHelper* watering_record_helper, ADCHardware* adc_hardware, double water_amount, std::string scheduled_freq, std::string scheduled_time) {
    watering_record_helper_ptr = watering_record_helper;
    adc_hardware_ptr = adc_hardware;
    th = new std::thread(this->pump_thread_main, water_amount, scheduled_freq, scheduled_time);
}

PumpThread::~PumpThread() {
    //stop the thread
    stop_thread = true;
    th->join();
    delete th;
    th = nullptr;
}

int PumpThread::pump_thread_main(double water_amount, std::string scheduled_freq, std::string scheduled_time) {
    //Get the current date and time
    time_point start_time_point = std::chrono::system_clock::now();
    std::time_t start_c = std::chrono::system_clock::to_time_t(start_time_point);
    std::tm* ltm_start = std::localtime(&start_c);  //local time

    /*
    int year = 1900 + ltm->tm_year;
    int month = 1 + ltm->tm_mon;    // tm_mon是从0开始的月份（0代表1月）
    int mday = ltm->tm_mday;         // tm_mday是日期（1-31）
    */
    int start_hour = ltm_start->tm_hour;    //hour
    int start_min = ltm_start->tm_min;   //minute
    
    //Get the scheduled hour and minute in integer
    int scheduled_hour = std::stoi(scheduled_time.substr(0, 2));
    int scheduled_min = std::stoi(scheduled_time.substr(2, 2));

    //Determine whether the first time of irrigation is in the next day
    bool is_next_day;
    if(scheduled_hour < start_hour) {
        is_next_day = true;
    } else if(scheduled_hour == start_hour) {
        if(scheduled_min <= start_min) {
            is_next_day = true;
        } else {
            is_next_day = false;
        }
    } else {
        is_next_day = false;
    }

    //Calculate the first duration
    double first_duration_hours;
    if(is_next_day == false) {
        first_duration_hours = (scheduled_min - start_min) / 60.0 + (scheduled_hour - start_hour);
    } else {
        first_duration_hours = (scheduled_min - start_min) / 60.0 + (scheduled_hour - start_hour) + 24;
    }
    auto first_duration = std::chrono::seconds((int)(first_duration_hours * 60 * 60));
    //time_point target_time;

    
    //Set the normal duration
    std::chrono::hours normal_duration;
    if(scheduled_freq == FREQ_EVERY_DAY) {
        normal_duration = std::chrono::hours(HOURS_ONE_DAY);
    } else if(scheduled_freq == FREQ_EVERY_TWO_DAYS) {
        normal_duration = std::chrono::hours(HOURS_TWO_DAYS);
    } else if(scheduled_freq == FREQ_EVERY_THREE_DAYS) {
        normal_duration = std::chrono::hours(HOURS_THREE_DAYS);
    } else {
        normal_duration = std::chrono::hours(HOURS_ONE_WEEK);
    }

    //Run the iterations
    //First duration (maybe < 1 day)
    while(true) {
        auto now = std::chrono::system_clock::now();
        if(now - start_time_point >= first_duration) { //If the duration has passed
            run_pump(water_amount, now);
            break;
        }
        if(water_immediately) {
            run_pump(water_amount, now);
            water_immediately = false;
        }
        if(stop_thread) {
            return 0;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));   //Sleep to avoid 
    }

    //The rest durations
    while(true) {
        start_time_point = std::chrono::system_clock::now();
        while(true) {
            auto now = std::chrono::system_clock::now();
            if(now - start_time_point >= normal_duration) {
                run_pump(water_amount, now);
                break;
            }
            if(water_immediately) {
                run_pump(water_amount, now);
                water_immediately = false;
            }
            if(stop_thread) {
                return 0;
            }
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
    return 0;
    

}

//Watering the plant and write the data to the database
int PumpThread::run_pump(double water_amount, std::chrono::system_clock::time_point now) {
    gpioSetMode(PUMP_PIN, PI_OUTPUT);

    //1. Determine whether water tank has enough water
    double water_level = adc_hardware_ptr->read_water_level();
    if(water_level < 20.0) {
        return 1;
    }

    //2. Start the pump
    gpioWrite(PUMP_PIN, PI_ON);  //Set the pin to high voltage 
    gpioDelay(500000 * 10); //Wait 500ms*10=5s. The high voltage will be there for 5s.
    gpioWrite(PUMP_PIN, PI_OFF);
 
    //3. Extract date (YYYYMMDD) from the timepoint parameter
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm;
    localtime_r(&now_time_t, &now_tm);  // Thread safe version of localtime()
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y%m%d", &now_tm);    //Format to YYYYMMDD
    std::string date_str(buffer);

    //4. Write the watering info into the watering record database
    //times_of_watering += 1, amount_of_watering += water_amount
    watering_record_helper_ptr->update_record(date_str, COL_TIMES_OF_WATERING, "1", true);
    watering_record_helper_ptr->update_record(date_str, COL_AMOUNT_OF_WATERING, std::to_string(water_amount), true);

    return 0;
}