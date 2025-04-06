#ifndef THREADOBJECTS_HPP
#define THREADOBJECTS_HPP

typedef struct thread_objects {
    PumpThread* pump_thread_obj;
    SoilMoistureThread* soil_moisture_thread_obj;
    NetworkingThread* net_thread_obj;
} thread_objects;

#endif