#include "server/home_irrigation_server.hpp"

int HomeIrrigationServer::init_server() {

    //Init pigpio library
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialise pigpio library" << std::endl;
        return 1;
    }
    return 0;
}