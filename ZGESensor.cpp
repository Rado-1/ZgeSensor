/*
ZGESensor Library
Copyright (c) 2013 Radovan Cervenka

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
*/

/// The main file used to compile Android shared library

// Definitions
#define export extern "C"

#define DONE 0
#define ERROR -1
#define NUMBER_OF_SENSORS 13

#define LOOPER_ID 1

// Includes
#include <cstdlib>
#include <android/sensor.h>
#include <android/looper.h>

// Globals
ASensorManager* sensorManager = ASensorManager_getInstance();
ASensorEventQueue* sensorEventQueue;
struct SensorItem {
    const ASensor* sensor;
    float x, y, z;
} sensors [NUMBER_OF_SENSORS];
ASensorEvent event;

// Public functions

// Initialize the library.
export void sensorInitLib(){
    // get looper
    ALooper* looper = ALooper_forThread();
    if(looper == NULL)
        looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);

    // initialize event queue
    sensorEventQueue = ASensorManager_createEventQueue(sensorManager, looper, LOOPER_ID, NULL, NULL);

    // clear array of sensors
    for(int i = 0; i < NUMBER_OF_SENSORS; i++) sensors[i].sensor = NULL;
}

// Stop the library usage. Returns a negative error code on failure.
export int sensorStopLib(){
    return ASensorManager_destroyEventQueue(sensorManager, sensorEventQueue);
}

// Start to use sensor of the specified type. Returns a negative error code on failure.
export int sensorUse(int type){

    if(sensors[type].sensor == NULL){
        // create and enable a new sensor
        const ASensor* sensor = ASensorManager_getDefaultSensor(sensorManager, type);
        if(sensor == NULL) return ERROR;
        sensors[type].sensor = sensor;
        ASensorEventQueue_enableSensor(sensorEventQueue, sensor);
    }

    return DONE;
}

// Set the delivery rate of events in microseconds for a sensor of the given type.
// Returns a negative error code on failure.
export int sensorSetEventRate(int type, float eventRate){
    return ASensorEventQueue_setEventRate(sensorEventQueue, sensors[type].sensor, eventRate);
}

// Disable a sensor of the specified type. Returns a negative error code on failure.
export int sensorDisable(int type){
    return ASensorEventQueue_disableSensor(sensorEventQueue, sensors[type].sensor);
}

// Enable a sensor of the specified type. Returns a negative error code on failure.
export int sensorEnable(int type){
    return ASensorEventQueue_enableSensor(sensorEventQueue, sensors[type].sensor);
}

// Update actual data for all the used sensors. Returns number of processes events.
export int sensorUpdateData(){
    int numberOfEvents = 0;

    while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0){
        sensors[event.type].x = event.vector.x;
        sensors[event.type].y = event.vector.y;
        sensors[event.type].z = event.vector.z;
        numberOfEvents++;
    }

    return numberOfEvents;
}

/*
 * Get actual data for the selected sensor type.
 * Vector sensors, such as accelerometer, magnetic field, or
 * gyroscope use x, y and z values. Scalar sensors, such as
 * light, proximity or temperature, use just x value.
 */
export void sensorGetData(int type, float &x, float &y, float &z){
    x = sensors[type].x;
    y = sensors[type].y;
    z = sensors[type].z;
}

// Return name of a sensor of the specified type.
export const char* sensorGetName(int type){
	return ASensor_getName(sensors[type].sensor);
}

// Return vendor's name of a sensor of the specified type.
export const char* sensorGetVendor(int type){
	return ASensor_getVendor(sensors[type].sensor);
}

// Return resolution of a sensor of the specified type.
export float sensorGetResolution(int type){
	return ASensor_getResolution(sensors[type].sensor);
}

/*
 * Return the minimum delay allowed between events in microseconds for
 * a sensor of the specified type.
 */
export float sensorGetMinDelay(int type){
	return ASensor_getMinDelay(sensors[type].sensor);
}
