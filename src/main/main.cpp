#include "main/home_irrigation_server.hpp"

int main() {
    HomeIrrigationServer* server = new HomeIrrigationServer();
    server->server_init();
    
    return 0;
}