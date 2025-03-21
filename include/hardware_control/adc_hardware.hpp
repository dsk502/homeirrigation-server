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
class ADCCommon {
public:
    int m_handle;

    int init_gpio();

private:
    //Mutex
    std::mutex m_i2c_mutex;

    int i2c_open(int bus, int address);
    int read_pcf8591_channel(int channel);
};

class SoilMoistureSensor {
    ADCCommon* m_adc_common;

    SoilMoistureSensor(ADCCommon* adc_common) {
        m_adc_common = adc_common;
    }

    double read_soil_humidity();
};

class WaterLevelSensor {
public:
    ADCCommon* m_adc_common;

    WaterLevelSensor(ADCCommon* adc_common) {
        m_adc_common = adc_common;
    }

    double read_water_level();
}