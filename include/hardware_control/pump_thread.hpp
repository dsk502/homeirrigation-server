#include <pigpio.h>
#include <chrono>
#define PUMP_PIN 18

#define FREQ_EVERY_DAY "1"
#define FREQ_EVERY_TWO_DAYS "2"
#define FREQ_EVERY_THREE_DAYS "3"
#define FREQ_EVERY_WEEK "4"
#define HOURS_ONE_DAY 24
#define HOURS_TWO_DAYS 48
#define HOURS_THREE_DAYS 72
#define HOURS_ONE_WEEK 168

class PumpThread {
    static bool water_immediately = false;
    static bool stop_thread = false;
    static int run_pump();
    static int pump_thread_schedule(std::string scheduled_freq, std::string scheduled_time, std::chrono::system_clock::time_point start_time_point, int start_hour, int start_min);


private:
    static std::mutex mtx;
};