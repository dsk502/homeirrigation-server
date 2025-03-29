#include "hardware_control/pump_thread.hpp"
#include "hardware_control/adc_hardware.hpp"
#include "database/sqlite_database.hpp"
#include "database/server_info_database_helper.hpp"
#include "database/watering_record_helper.hpp"

//The resources used in the server program

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
    std::string m_server_pubkey;
    std::string m_server_prikey;
    
    //Server info
    server_info* m_server_info;

    //Thread class objects
    PumpThread* m_pump_thread_obj;
    

    int server_init();

private:
    std::string HomeIrrigationServer::get_raspberry_pi_id();
};

/*
int main() {
    HomeIrrigationServer server = new HomeIrrigationServer();
    server.server_main();
    return 0;
}*/

typedef struct date_time {
    
}