#include "include/hardware_control/adc_hardware.hpp"

int ADCCommon::init_gpio() {

    int handle = i2c_open(I2C_BUS, PCF8591_ADDRESS);
    if (handle < 0) {
        //gpioTerminate();
        return 1;
    }
    m_handle = handle;
    return 0;
}

//Initialize i2c communication
int ADCCommon::i2c_open(int bus, int address) {
    char device[16];
    sprintf(device, "/dev/i2c-%d", bus);
    int handle = ::open(device, O_RDWR);
    if (handle < 0) {
        std::cerr << "Failed to open I2C device" << std::endl;
        return -1;
    }
    if (ioctl(handle, I2C_SLAVE, address) < 0) {
        std::cerr << "Failed to set I2C device address" << std::endl;
        ::close(handle);
        return -1;
    }
    return handle;
}

//Read PCF8591 channel data
int ADCCommon::read_pcf8591_channel(int channel) {
    std::lock_guard<std::mutex> lock(i2c_mutex); // auto lock and unlock
    char command = channel; // set channel
    ::write(m_handle, &command, 1);
    char data;
    ::read(m_handle, &data, 1);
    return static_cast<int>(data);
}

double SoilMoistureSensor::read_soil_humidity() {
   
    int soil_humidity_value = m_adc_common->read_pcf8591_channel(m_adc_common->m_handle, SOIL_HUMIDITY_CHANNEL);
    double soil_humidity_percentage = (1023 - soil_humidity_value) * 100.0 / 1023;
    return soil_humidity_percentage;
    
}

double WaterLevelSensor::read_water_level() {
    int water_level_value = m_adc_common->read_pcf8591_channel(m_adc_common->m_handle, WATER_LEVEL_CHANNEL);
    double water_level_percentage = water_level_value * 100.0 / 1023;
    return water_level_percentage;
    
}

