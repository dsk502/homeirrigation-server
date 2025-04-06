#ifndef THREADOBJECTS_HPP
#define THREADOBJECTS_HPP

#include "hardware_control/pump_thread.hpp"
#include "hardware_control/soil_moisture_thread.hpp"
#include "networking/networking_thread.hpp"

typedef struct thread_objects {
    PumpThread* pump_thread_obj;
    SoilMoistureThread* soil_moisture_thread_obj;
    NetworkingThread* net_thread_obj;
} thread_objects;

#endif