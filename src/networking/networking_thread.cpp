#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include "networking/networking_thread.hpp"
#include "hardware_control/soil_moisture_thread.hpp"
#include "database/server_info_database_helper.hpp"
#include "crypto/rsa_utils.hpp"

#define ENCRYPTED_TAG (char)0x02
#define UNENCRYPTED_TAG (char)0x01

//std::string server_prikey;
//Char array copy method similar to Java's System.arraycopy()

void char_array_copy(const char* src, int src_pos, char* dest, int dest_pos, int len) {
    for(int i = dest_pos; i < dest_pos + len; i++) {
        dest[i] = src[i + src_pos - dest_pos];
    }
}

//Find the index of a character in a char array

int find_char_index(char* char_array, int char_array_len, char ch) {
    for(int i = 0; i < char_array_len; i++) {
        if(char_array[i] == ch) {
            return i;
        }
    }
    return -1;
}

std::string unpack_unencrypted_message(char* receive_buffer, int receive_len) {
    int end_tag_index = find_char_index(receive_buffer, receive_len, '\n');
    if(end_tag_index == -1) {
        return "";
    }

    char* message_block = new char[end_tag_index];
    char_array_copy(receive_buffer, 0, message_block, 1, end_tag_index);
    std::string unpacked_message = std::string(message_block, end_tag_index);
    return unpacked_message;
}

std::string unpack_encrypted_message(char* receive_buffer, int receive_len, std::string key_for_decryption) {
    std::string message_cipher = unpack_unencrypted_message(receive_buffer, receive_len);
    RSA* server_prikey_loaded = RSAUtils::load_base64_der_private_key(key_for_decryption);
    std::string message_pt = RSAUtils::rsa_decrypt(message_cipher, server_prikey_loaded);
    //std::string message_pt = "";
    return message_pt;
}

char* pack_message(std::string message, bool is_encrypted, int* packed_bytes_len_ret, std::string key_for_encryption) {
    int packed_bytes_len = message.length() + 2;
    char* packed_bytes = new char[packed_bytes_len];
    if(is_encrypted) {
        RSA* client_pubkey_loaded = RSAUtils::load_base64_der_public_key(key_for_encryption);
        std::string encrypted_message = RSAUtils::rsa_encrypt(client_pubkey_loaded, message);
        char_array_copy(encrypted_message.c_str(), 0, packed_bytes, 1, encrypted_message.length());
        packed_bytes[0] = ENCRYPTED_TAG;
    } else {
        packed_bytes[0] = UNENCRYPTED_TAG;
        char_array_copy(message.c_str(), 0, packed_bytes, 1, message.length());
    }
    packed_bytes[packed_bytes_len - 1] = '\n';
    *packed_bytes_len_ret = packed_bytes_len;
    return packed_bytes;
}

std::string extract_command(std::string message_pt) {
    std::string result;

    // 查找 '(' 的位置
    size_t pos = message_pt.find('(');

    if (pos != std::string::npos) {
        // 如果找到了 '('，提取从开头到 '(' 之前的部分
        result = message_pt.substr(0, pos);
    } else {
        // 如果没有找到 '('，提取整个字符串
        result = "";
    }
    return result;
}

std::vector<std::string> extract_params(std::string message_pt) {
    std::vector<std::string> params;

    // 查找 '(' 和 ')' 的位置
    size_t startPos = message_pt.find('(');
    size_t endPos = message_pt.find(')');

    // 检查是否找到了括号
    if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
        // 提取括号之间的内容
        std::string content_between_braces = message_pt.substr(startPos + 1, endPos - startPos - 1);

        // 使用 stringstream 按逗号分割
        std::stringstream ss(content_between_braces);
        
        std::string item;

        while (std::getline(ss, item, ',')) {
            params.push_back(item);
        }
    }
    return params;
}

int NetworkingThread::networking_thread_main(bool* is_added, server_info* server_info, ServerInfoDatabaseHelper* server_info_db_helper, WateringRecordHelper* watering_record_helper, PumpThread*& pump_thread_obj, std::thread*& pump_thread, SoilMoistureThread*& soil_moisture_thread_obj, std::thread*& soil_moisture_thread, std::string& server_pubkey, std::string& server_prikey) {    //pump_thread is a reference to the pointer pointing to PumpThread. This can pass the pointer by its address (the pointer of pointer).
    std::string server_id;

    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char receive_buffer[1024] = {0};
    //const char* response = "Hello from server";

    // 创建TCP套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址和端口
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // 绑定套接字到地址和端口
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 开始监听
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port 8080..." << std::endl;

    while(true) {
        // 接受客户端连接
        if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        std::cout << "Client connected." << std::endl;

        // 循环接收和发送消息
        //while (true) {
        memset(receive_buffer, 0, sizeof(receive_buffer));  // 清空缓冲区
        int receive_len = read(client_socket, receive_buffer, 1024);   //The data from client will be in buffer
        if (receive_len <= 0) {
            std::cout << "Client disconnected." << std::endl;
            break;  // 客户端断开连接
        }

        //Process the data from buffer
        if(receive_buffer[0] == ENCRYPTED_TAG) {   //Encrypted message
           
            std::string received_message = unpack_encrypted_message(receive_buffer, receive_len, server_prikey);
            std::string recv_command = extract_command(received_message);
            
            if(recv_command == "delete_device") {
                //Begin delete device
                if(*is_added == true) {
                    //Clear server_info
                    server_info_db_helper->clear_server_info();

                    //Exit the pump control thread
                    pump_thread_obj->stop_thread = true;
                    pump_thread->join();
                    delete pump_thread;
                    pump_thread = nullptr;
                    delete pump_thread_obj;
                    pump_thread_obj = nullptr;

                    //Exit the soil moisture thread
                    soil_moisture_thread_obj->stop_thread = true;
                    soil_moisture_thread->join();
                    delete soil_moisture_thread;
                    soil_moisture_thread = nullptr;
                    delete soil_moisture_thread_obj;
                    soil_moisture_thread_obj = nullptr;

                    //Clear watering info
                    watering_record_helper->clear_record();

                    //Clear keys
                    RSAUtils::write_key_to_file(true, "");
                    RSAUtils::write_key_to_file(false, "");
                    server_pubkey = "";
                    server_prikey = "";

                    //Reply "finish_del_device_server()"
                    std::string sending_message = "finish_del_device_server()";
                    int sending_bytes_len;
                    char* sending_bytes = pack_message(sending_message, true, &sending_bytes_len, server_info->client_pubkey);
                    send(client_socket, sending_bytes, sending_bytes_len, 0);

                } else {
                    //Error
                }
                //End delete device
            } else if(recv_command == "edit_mode") {
                //Begin edit mode
                std::vector<std::string> params = extract_params(received_message);
                std::string new_mode = params[0];
                std::string new_water_amount = params[1];
                std::string new_scheduled_freq = params[2];
                std::string new_scheduled_time = params[3];

                //Edit info in database
                server_info_db_helper->update_mode(new_mode, new_water_amount, new_scheduled_freq, new_scheduled_time);

                //Edit info in memory
                server_info->mode = new_mode;
                server_info->water_amount = new_water_amount;
                server_info->scheduled_freq = new_scheduled_freq;
                server_info->scheduled_time = new_scheduled_time;

                //Restart the pump thread
                pump_thread_obj->stop_thread = true;
                pump_thread->join();
                delete pump_thread;
                pump_thread = nullptr;
                delete pump_thread_obj;
                pump_thread_obj = nullptr;

                pump_thread_obj = new PumpThread();
                pump_thread = new std::thread(pump_thread_obj->pump_thread_main, std::stod(water_amount), scheduled_freq, scheduled_time);

                //Reply "finish_edit_server"
                std::string sending_message = "finish_edit_server()";
                int sending_bytes_len;
                char* sending_bytes = pack_message(sending_message, true, &sending_bytes_len, server_info->client_pubkey);
                send(client_socket, sending_bytes, sending_bytes_len, 0);
                //End edit mode
            } else if(recv_command == "watering_now"){

                pump_thread_obj->water_immediately = true;

                //Reply "watering_succeed"
                std::string sending_message = "watering_succeed()";
                int sending_bytes_len;
                char* sending_bytes = pack_message(sending_message, true, &sending_bytes_len, server_info->client_pubkey);
                send(client_socket, sending_bytes, sending_bytes_len, 0);

            } else if(recv_command == "download_stat"){
                //Send the watering_record database file to the client
                



            } else if(recv_command == "del_stat"){  //Delete all data
                watering_record_helper->clear_record();

                //Reply "finish_del_stat_server"
                std::string sending_message = "finish_del_stat_server()";
                int sending_bytes_len;
                char* sending_bytes = pack_message(sending_message, true, &sending_bytes_len, server_info->client_pubkey);
                send(client_socket, sending_bytes, sending_bytes_len, 0);
            } else {
                //Error: invalid message
            }

        } else if(receive_buffer[0] == UNENCRYPTED_TAG) {    //Unencrypted message
            std::string received_message = unpack_unencrypted_message(receive_buffer, receive_len);
            std::string recv_command = extract_command(received_message);

            if(recv_command == "add_device") {
                //Begin add device
                if(*is_added == false) {
                
                    //If the command is "add_device"
                    std::vector<std::string> params = extract_params(received_message);
                    if(params.size() != 1) {
                        //Error: invalid argument list
                    }
                    std::string timestamp = params[0];   //Store the timestamp

                    //Reply "key_exchange_server(server_id)"
                    std::string sending_message = "key_exchange_server(" + server_id + ")";
                    int sending_bytes_len;
                    char* sending_bytes = pack_message(sending_message, false, &sending_bytes_len);
                    send(client_socket, sending_bytes, sending_bytes_len, 0);

                    //Receive "key_exchange_client(client_pubkey)"
                    memset(receive_buffer, 0, sizeof(receive_buffer));
                    receive_len = read(client_socket, receive_buffer, 1024);
                    if(receive_buffer[0] == (char)0x01) {
                        //Error: invalid message
                    }
                    received_message = unpack_unencrypted_message(receive_buffer, receive_len);
                    recv_command = extract_command(received_message);
                    if(recv_command != "key_exchange_client") {
                        //Error: message sequence error
                    }
                    params = extract_params(received_message);
                    if(params.size() != 1) {
                        //Error: invalid argument list
                    }
                    std::string client_pubkey = params[0];

                    //Reply "request_add_param(server_id)"
                    sending_message = "request_add_param(" + server_id + ")";        
                    sending_bytes = pack_message(sending_message, true, &sending_bytes_len, server_info->client_pubkey);
                    send(client_socket, sending_bytes, sending_bytes_len, 0);

                    //Receive "reply_add_param(mode, water_amount, scheduled_freq, scheduled_time)"
                    memset(receive_buffer, 0, sizeof(receive_buffer));
                    receive_len = read(client_socket, receive_buffer, 1024);
                    if(receive_buffer[0] != (char)0x02) {
                        //Error: invalid message
                    }
                    received_message = unpack_encrypted_message(receive_buffer, receive_len, server_prikey);
                    recv_command = extract_command(received_message);
                    
                    if(recv_command != "reply_add_param") {
                        //Error: invalid message
                    }
                    params = extract_params(received_message);
                    if(params.size() != 4) {
                        //Error: invalid argument list
                    }
                    std::string mode = params[0];
                    std::string water_amount = params[1];
                    std::string scheduled_freq = params[2];
                    std::string scheduled_time = params[3];

                    //Reply "finish_add_server()"
                    sending_message = "finish_add_server()";
                    sending_bytes = pack_message(sending_message, true, &sending_bytes_len, server_info->client_pubkey);
                    send(client_socket, sending_bytes, sending_bytes_len, 0);

                    //Receive "finish_add_client()"
                    memset(receive_buffer, 0, sizeof(receive_buffer));
                    receive_len = read(client_socket, receive_buffer, 1024);
                    if(receive_buffer[0] != (char)0x02) {
                        //Error: invalid message
                    }
                    received_message = unpack_encrypted_message(receive_buffer, receive_len, server_prikey);
                    recv_command = extract_command(received_message);
                    
                    if(recv_command != "finish_add_client") {
                        //Error: invalid message
                    }

                    //Store the data to the database
                    server_info_db_helper->insert_record("", client_pubkey, timestamp, mode, water_amount, scheduled_freq, scheduled_time);

                    //Update the server info in memory
                    server_info->client_pubkey = client_pubkey;
                    server_info->client_add_time = timestamp;
                    server_info->mode = mode;
                    server_info->water_amount = water_amount;
                    server_info->scheduled_freq = scheduled_freq;
                    server_info->scheduled_time = scheduled_time;

                    *is_added = true;

                    //Start pump control thread and soil moisture thread
                    pump_thread_obj = new PumpThread();
                    pump_thread = new std::thread(pump_thread_obj->pump_thread_main, std::stod(water_amount), scheduled_freq, scheduled_time);
                    soil_moisture_thread_obj = new SoilMoistureThread();
                    soil_moisture_thread = new std::thread(soil_moisture_thread_obj->soil_moisture_thread_main);

                } else {
                    //Error: device already added
                }
            //End add device
            } else {
                //Error: invalid message
            }
        } else {
            //Error: invalid message
        }

        // 向客户端发送响应
        //send(client_socket, response, strlen(response), 0);
        //std::cout << "Response sent to client." << std::endl;
        close(client_socket);
    }

    // 关闭套接字
    
    close(server_fd);

    return 0;
}

