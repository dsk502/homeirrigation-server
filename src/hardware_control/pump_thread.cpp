#include "hardware_control/pump_thread.hpp"

int PumpThread::pump_thread_schedule(std::string scheduled_freq, std::string scheduled_time, std::chrono::system_clock::time_point start_time_point, int start_hour, int start_min) {
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
        duration_hours = (scheduled_min - start_min) / 60.0 + (scheduled_hour - start_hour);
    } else {
        duration_hours = (scheduled_min - start_min) / 60.0 + (scheduled_hour - start_hour) + 24;
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
        if(std::chrono::system_clock::now() - start_time_point >= first_duration) { //If the duration has passed
            run_pump();
            break;
        }
        if(water_immediately) {
            run_pump();
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
            if(std::chrono::system_clock::now() - start_time_point >= normal_duration) {
                run_pump();
                break;
            }
            if(water_immediately) {
                run_pump();
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
int PumpThread::run_pump(std::chrono::system) {
    gpioSetMode(PUMP_PIN, PI_OUTPUT);

    //Control the pump
    gpioWrite(PUMP_PIN, PI_ON);  //Set the pin to high voltage 
    gpioDelay(500000 * 10); //Wait 500ms*10=5s. The high voltage will be there for 5s.
    gpioWrite(PUMP_PIN, PI_OFF);

    //Write the watering info into the watering record database
    
    return 0;
}