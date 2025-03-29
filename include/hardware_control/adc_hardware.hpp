#include <iostream>
#include <pigpio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <mutex>
#include <thread>

#define I2C_BUS 1
#define PCF8591_ADDRESS 0x48
#define WATER_LEVEL_CHANNEL 1
#define SOIL_HUMIDITY_CHANNEL 2

//class for hardware connecting to the analog to digital converter
//They are water level sensor and soil moisture sensor
class ADCHardware {
public:

    int init_gpio();    //This is like the constructor

    double read_soil_humidity();
    double read_water_level();

private:
    //Mutex
    std::mutex m_i2c_mutex;

    int m_handle;

    int i2c_open(int bus, int address);
    int read_pcf8591_channel(int channel);
    
};