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

//Mutex
std::mutex i2c_mutex;

float soilHumidityPercentage(int soilHumidityValue) {
    float result = (1023 - soilHumidityValue) * 100.0 / 1023;
    return result;
}

//Initialize i2c communication
int i2cOpen(int bus, int address) {
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
int readPCF8591Channel(int handle, int channel) {
    std::lock_guard<std::mutex> lock(i2c_mutex); // auto lock and unlock
    char command = channel; // set channel
    ::write(handle, &command, 1);
    char data;
    ::read(handle, &data, 1);
    return static_cast<int>(data);
}

void readSoilHumidity(int handle) {
    while (true) {
        int soilHumidityValue = readPCF8591Channel(handle, SOIL_HUMIDITY_CHANNEL);
        //float soilHumidityPercentage = (1023 - soilHumidityValue) * 100.0 / 1023;
        std::cout << "Soil Humidity Value: " << soilHumidityValue << std::endl;
        std::cout << "Soil Humidity: " << soilHumidityPercentage(soilHumidityValue) << "%" << std::endl;
        sleep(1); // wait for 1 second
    }
}

void readWaterLevel(int handle) {
    while (true) {
        int waterLevelValue = readPCF8591Channel(handle, WATER_LEVEL_CHANNEL);
        float waterLevelPercentage = waterLevelValue * 100.0 / 1023;
        std::cout << "Water Level: " << waterLevelPercentage << "%" << std::endl;
        sleep(1); // wait for 1 second
    }
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialise pigpio library" << std::endl;
        return 1;
    }

    int handle = i2cOpen(I2C_BUS, PCF8591_ADDRESS);
    if (handle < 0) {
        gpioTerminate();
        return 1;
    }

    //Create two threads to read soil moisture and water level seperately
    std::thread soilThread(readSoilHumidity, handle);
    std::thread waterThread(readWaterLevel, handle);

    soilThread.join();
    waterThread.join();

    ::close(handle);
    gpioTerminate();
    return 0;
}

/*
    确保树莓派已启用I2C接口，并安装了pigpio库。
    根据实际连接的传感器和PCF8591模块，可能需要调整代码中的通道号和I2C地址。
    如果传感器的输出范围不是0-1023，需要根据实际范围调整计算公式。
*/