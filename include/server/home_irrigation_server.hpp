#include "hardware_control/pump.hpp"
#include "hardware_control/adc_hardware.hpp"
#include "database/sqlite_database_helper.hpp"
#include "watering_record/watering_record_helper.hpp"

class HomeIrrigationServer {

    //Hardware
    Pump* m_pump;
    SoilMoistureSensor* m_soil_moisture_sensor;
    WaterLevelSensor* m_water_level_sensor;

    //Database
    SQLiteDatabaseHelper* m_database_helper;

    //Watering Record
    WateringRecordHelper* m_watering_record_helper;

    int init_server();
};