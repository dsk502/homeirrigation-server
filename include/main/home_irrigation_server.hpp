#ifndef HOMEIRRIGATIONSERVER_HPP
#define HOMEIRRIGATIONSERVER_HPP

#include "hardware_control/pump_thread.hpp"
#include "hardware_control/adc_hardware.hpp"
#include "hardware_control/soil_moisture_thread.hpp"

#include "database/sqlite_database.hpp"
#include "database/server_info_database_helper.hpp"
#include "database/watering_record_helper.hpp"
#include "crypto/rsa_utils.hpp"
#include "networking/networking_thread.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <sqlite3.h>
#include <chrono>
#include <algorithm>

//The resources used in the server program

typedef struct thread_objects {
    PumpThread* pump_thread_obj;
    SoilMoistureThread* soil_moisture_thread_obj;
    NetworkingThread* net_thread_obj;
} thread_objects;

class HomeIrrigationServer {
public:
    //Resources
    //Server ID
    std::string m_server_id;

    //Status
    bool m_is_added;

    //Hardware
    ADCHardware* m_adc_hardware;

    //Database helpers
    ServerInfoDatabaseHelper* m_server_info_database_helper;
    WateringRecordHelper* m_watering_record_helper;

    //Server keys
    //std::string m_server_pubkey;
    //std::string m_server_prikey;
    
    //Server info
    server_info* m_server_info;

    //Thread class objects struct
    thread_objects* m_thread_objs;

    //Thread variables
    //std::thread* m_pump_thread;
    //std::thread* m_soil_moisture_thread;
    //std::thread* m_net_thread;

    int server_init();

private:
    std::string get_raspberry_pi_id();
};

#endif