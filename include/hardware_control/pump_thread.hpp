#ifndef PUMPTHREAD_HPP
#define PUMPTHREAD_HPP

#include <chrono>
#include <thread>
#include <atomic>
#include "database/watering_record_helper.hpp"
#include "database/server_info_database_helper.hpp"
#include "adc_hardware.hpp"

#define PUMP_PIN 18

#define FREQ_EVERY_DAY "1"
#define FREQ_EVERY_TWO_DAYS "2"
#define FREQ_EVERY_THREE_DAYS "3"
#define FREQ_EVERY_WEEK "4"
#define HOURS_ONE_DAY 24
#define HOURS_TWO_DAYS 48
#define HOURS_THREE_DAYS 72
#define HOURS_ONE_WEEK 168

class PumpThread {
public:
    std::atomic<bool> water_immediately; //Will be edited by multiple threads, so atomic
    
    PumpThread(WateringRecordHelper* watering_record_helper, ADCHardware* adc_hardware);
    void create_thread(const server_info* server_information);
    ~PumpThread();

    int run_pump(double water_amount, std::chrono::system_clock::time_point now);
    int pump_thread_main(const server_info* server_information);

private:
    WateringRecordHelper* watering_record_helper_ptr;
    ADCHardware* adc_hardware_ptr;

    bool stop_thread = false;
    std::thread* th;

    int high_voltage_time(double water_amount);
};

#endif