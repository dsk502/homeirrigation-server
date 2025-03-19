#include <iostream>
#include <pigpio.h>
#include <unistd.h>

#define I2C_ADDR 0x48 // PCF8591 的 I2C 地址[^24^]
#define CHANNEL 0x42  // 选择 AIN2 通道[^24^]

int main() {
    // 初始化 pigpio
    if (gpioInitialise() < 0) {
        std::cerr << "Failed to initialize pigpio" << std::endl;
        return -1;
    }

    // 打开 I2C 总线
    int handle = i2cOpen(1, I2C_ADDR, 0); // 使用 I2C 总线 1[^24^]
    if (handle < 0) {
        std::cerr << "Failed to open I2C handle" << std::endl;
        gpioTerminate();
        return -1;
    }

    std::cout << "Reading soil moisture sensor values..." << std::endl;

    while (true) {
        // 写入通道选择命令
        i2cWriteByte(handle, CHANNEL); // 选择 AIN0[^24^]

        // 读取传感器值
        int value = i2cReadByte(handle); // 读取一个字节[^24^]

        // 打印读取的值
        std::cout << "Soil Moisture Sensor Value: " << value << std::endl;

        // 等待 1 秒
        sleep(1);
    }

    // 关闭 I2C 总线
    i2cClose(handle);

    // 清理并退出
    gpioTerminate();
    return 0;
}