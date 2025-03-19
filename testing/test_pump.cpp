#include <iostream>
#include <pigpio.h>

int main() {
    //Initialize pigpio library
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialize pigpio library." << std::endl;
        return 1;
    }

    //Set gpio pin number and mode 
    int pin = 18;
    gpioSetMode(pin, PI_OUTPUT);

    //Control the pump
    gpioWrite(pin, PI_ON);  //Set the pin to high voltage 
    gpioDelay(500000 * 10); //Wait 500ms*10=5s. The high voltage will be there for 5s.
    gpioWrite(pin, PI_OFF); //Set the pin to low voltage
    gpioDelay(500000);      //Wait 500ms


    //Clean and exit
    gpioTerminate();
    return 0;
}