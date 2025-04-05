#ifndef SOILMOISTURETHREAD_HPP
#define SOILMOISTURETHREAD_HPP

#include "adc_hardware.hpp"
#include "database/watering_record_helper.hpp"
#include <thread>
#include <chrono>
#include <ctime>
#include <cstring>

class SoilMoistureThread {
public:
    bool stop_thread = false;
    std::thread* th;

    SoilMoistureThread(ADCHardware* adc_hardware, WateringRecordHelper* watering_record_helper);
    void create_thread();
    ~SoilMoistureThread();
    
    int soil_moisture_thread_main();
private:
    ADCHardware* adc_hardware_ptr;
    WateringRecordHelper* watering_record_helper_ptr;
};

#endif