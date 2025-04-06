#ifndef NETWORKINGTHREAD_HPP
#define NETWORKINGTHREAD_HPP

#include "hardware_control/soil_moisture_thread.hpp"
#include "database/server_info_database_helper.hpp"
#include "crypto/rsa_utils.hpp"
#include "crypto/aes_utils.hpp"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include "hardware_control/pump_thread.hpp"
#include "hardware_control/adc_hardware.hpp"
#include "../home_irrigation_server.hpp"

class NetworkingThread {
public:
    std::thread* th;
    //bool stop_thread;

    int networking_thread_main(bool* is_added, std::string server_id, server_info* server_info, thread_objects* thread_objs);
    
    NetworkingThread(ServerInfoDatabaseHelper* server_info_db_helper, WateringRecordHelper* watering_record_helper, ADCHardware* adc_hardware);
    void create_thread(bool* is_added, std::string server_id, server_info* server_information, thread_objects* thread_objs);
    ~NetworkingThread();

private:
    WateringRecordHelper* watering_record_helper_ptr;
    ADCHardware* adc_hardware_ptr;
    ServerInfoDatabaseHelper* server_info_db_helper_ptr;

    int server_fd;
    int client_socket;

    void char_array_copy(const char* src, int src_pos, char* dest, int dest_pos, int len);
    int find_char_index(char* char_array, int char_array_len, char ch);
    std::string unpack_unencrypted_message(char* receive_buffer, int receive_len);
    std::string unpack_encrypted_message(char* receive_buffer, int receive_len);
    char* pack_message(std::string message, bool is_encrypted, int* packed_bytes_len_ret, std::string key_for_encryption);
    std::string extract_command(std::string message_pt);
    std::vector<std::string> extract_params(std::string message_pt);
    int sendAll(int sock, const char* buf, int len);
};

#endif