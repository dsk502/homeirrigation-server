#include "adc_hardware.hpp"
#include "database/watering_record_helper.hpp"
#include <chrono>
#include <ctime>

class SoilMoistureThread {
public:
    bool stop_thread = false;
    SoilMoistureThread(ADCHardware* adc_hardware, WateringRecordHelper* watering_record_helper);
    int soil_moisture_thread_main();
private:
    ADCHardware* adc_hardware_ptr;
    WateringRecordHelper* watering_record_helper_ptr;
};