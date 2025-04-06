#ifndef THREADOBJECTS_HPP
#define THREADOBJECTS_HPP

typedef struct hardware_thread_objects {
    PumpThread* pump_thread_obj;
    SoilMoistureThread* soil_moisture_thread_obj;
} hardware_thread_objects;

#endif