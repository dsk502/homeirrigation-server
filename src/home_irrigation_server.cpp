#include "home_irrigation_server.hpp"

std::string HomeIrrigationServer::get_raspberry_pi_id() {
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    std::string serial = "0000000000000000"; // 默认值

    if (cpuinfo.is_open()) {
        while (std::getline(cpuinfo, line)) {
            if (line.find("Serial") != std::string::npos) {
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    serial = line.substr(colonPos + 1);
                    serial.erase(serial.begin(), std::find_if(serial.begin(), serial.end(), [](unsigned char ch) {
                        return !std::isspace(ch);
                    }));
                    break;
                }
            }
        }
        cpuinfo.close();
    } else {
        std::cerr << "无法打开 /proc/cpuinfo 文件" << std::endl;
    }

    return serial;
}

int HomeIrrigationServer::server_init() {

    //1. Init pigpio library
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialise pigpio library" << std::endl;
        return 1;
    }
    return 0;

    //2. Check if server_id file exists
    //If yes, read the server id; If not, generate the file
    std::ifstream server_id_file_read("server_id.txt");
    if (server_id_file_read) {
        std::cout << "Server id file exist" << std::endl;
        std::getline(server_id_file_read, m_server_id);
        server_id_file_read.close();
    } else {
        std::cout << "Server id file does not exist" << std::endl;
        server_id_file_read.close();
        m_server_id = get_raspberry_pi_id();
        std::ofstream server_id_file_write("server_id.txt");
        server_id_file_write << m_server_id << std::endl;
        server_id_file_write.close();
    }

    //3. Init the server_info database if there is nothing in it.
    m_server_info_database_helper = new ServerInfoDatabaseHelper();
    m_server_info_database_helper->create_table_if_not_exist();

    //4. Init the ADC hardware
    m_adc_hardware = new ADCHardware();
    m_adc_hardware->init_gpio();

    //5. Read server_info database to determine whether this device is added by the client
    int num_of_records = m_server_info_database_helper->record_num();
    if(num_of_records == 1) {
        m_is_added == true;
    } else if(num_of_records == 0) {
        m_is_added == false;
    } else {
        //Server Error
        std::cout << "Server Error" << std::endl;
        return 1;
    }

    if(m_is_added) {   //If the device is added
        //Read the server info record (client_pubkey is in it)
        m_server_info = m_server_info_database_helper->get_server_info();
        
        //Read the keys
        m_server_pubkey = RSAUtils::read_key_from_file(true);
        m_server_prikey = RSAUtils::read_key_from_file(false);

        //Start the pump thread
        m_pump_thread_obj = new PumpThread(m_watering_record_helper, m_adc_hardware);
        //m_pump_thread_obj->th = new std::thread(m_pump_thread_obj->pump_thread_main, m_server_info->scheduled_freq, m_server_info->scheduled_time);
        //m_pump_thread = new std::thread(m_pump_thread_obj->pump_thread_main, m_server_info->scheduled_freq, m_server_info->scheduled_time);
        //m_pump_thread.detach();

        //Start the soil moisture thread
        m_soil_moisture_thread_obj = new SoilMoistureThread(m_adc_hardware, m_watering_record_helper);
        m_soil_moisture_thread_obj->th = new std::thread(m_soil_moisture_thread_obj->soil_moisture_thread_main);
        //m_soil_moisture_thread = new std::thread(m_soil_moisture_thread_obj->soil_moisture_thread_main);
        //m_soil_moisture_thread.detach();

    } else {    //If the device is not added
        //Do nothing
    }
    m_net_thread_obj = new NetworkingThread();
    m_net_thread = new std::thread(m_net_thread_obj->networking_thread_main);
    //m_net_thread.detach();
}