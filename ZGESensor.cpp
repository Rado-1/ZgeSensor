/*
ZgeSensor Library
Copyright (c) 2013-2015 Radovan Cervenka

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

// Includes

#include <cstdlib>
#include <android/sensor.h>
#include <android/looper.h>
#include <android/log.h>
#include <jni.h>


// Definitions

//#define LOGGING
#define LOG_TAG "ZgeSensor"

#ifdef LOGGING
#	define LOG_DEBUG(str) __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, str);
#else
#	define LOG_DEBUG(str)
#endif

#define export extern "C"

#define DONE 0
#define ERROR -1
#define NUMBER_OF_SENSORS 13

#define LOOPER_ID 3

//Additional sensor types; not in NDK, but available in SDK
enum {
    ASENSOR_TYPE_PRESSURE               = 6,
    ASENSOR_TYPE_GRAVITY                = 9,
    ASENSOR_TYPE_LINEAR_ACCELERATION    = 10,
    ASENSOR_TYPE_ROTATION_VECTOR        = 11,
    ASENSOR_TYPE_RELATIVE_HUMIDITY      = 12,
    ASENSOR_TYPE_AMBIENT_TEMPERATURE    = 13
};

// Globals

ASensorManager* sensorManager = ASensorManager_getInstance();
ASensorEventQueue* sensorEventQueue;
struct SensorItem {
    const ASensor* sensor;
    ASensorEvent event;
} sensors [NUMBER_OF_SENSORS];
ASensorEvent event;
void* sensor_data = malloc(1000);
int displayRotation;

// Private functions

// Process sensor events
static int getSensorEvents(int fd, int events, void* data){
	//LOG_DEBUG("event callback called");

    while(ASensorEventQueue_getEvents(sensorEventQueue, &event, 1) > 0)
        sensors[event.type].event = event;
    // to continue receiving events
    return 1;
}

// Public functions

// Initialize JNI; used to obtain display rotation
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){
	LOG_DEBUG("JNI_OnLoad called");

    /* Java code used to obtain display rotation (executed in ZgeActivity):
     *
     * WindowManager wm = (WindowManager) getSystemService(WINDOW_SERVICE);
     * Display disp = wm.getDefaultDisplay();
     * displayRotation = disp.getRotation();
     */

    /* REMARK: The following code is executed just once, at the library loading
     * time. When ZGE will allow to rotate screen, this code will just cache
     * classes, IDs and constants to global references and getDisplayRotation()
     * function will call methods to obtain rotation dynamically in runtime.
     */

    // get JNI environment
    JNIEnv *env;
    vm->GetEnv((void**) &env, JNI_VERSION_1_6);

    // get ZgeActivity instance and its class
    jclass zgeActivityCls = env->FindClass("org/zgameeditor/ZgeActivity");
    jfieldID zgeActivityID = env->GetStaticFieldID(zgeActivityCls, "zgeActivity", "Lorg/zgameeditor/ZgeActivity;");
    jobject zgeActivity = env->GetStaticObjectField(zgeActivityCls, zgeActivityID);

    // get WINDOW_SERVICE constant
    jclass contextCls = env->FindClass("android/content/Context");
    jfieldID windowServiceID = env->GetStaticFieldID(contextCls, "WINDOW_SERVICE", "Ljava/lang/String;");
    jstring windowServiceConst = (jstring) env->GetStaticObjectField(contextCls, windowServiceID);

    // get window manager and its class
    jmethodID getSystemServiceID = env->GetMethodID(zgeActivityCls, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jobject wm = env->CallObjectMethod(zgeActivity, getSystemServiceID, windowServiceConst);
    jclass wmCls = env->FindClass("android/view/WindowManager");

    // get default display and its class
    jmethodID getDefaultDisplayID = env->GetMethodID(wmCls, "getDefaultDisplay", "()Landroid/view/Display;");
    jobject disp = env->CallObjectMethod(wm, getDefaultDisplayID);
    jclass dispCls = env->FindClass("android/view/Display");

    // get display rotation
    jmethodID getRotationID = env->GetMethodID(dispCls, "getRotation", "()I");
    displayRotation = env->CallIntMethod(disp, getRotationID);

    LOG_DEBUG("JNI_OnLoad finished");

    return JNI_VERSION_1_6;
}

// Get rotation of display
export int getDisplayRotation(){
	LOG_DEBUG("getDisplayRotation called");
    return displayRotation;
}

// Initialize the library.
export void sensorInitLib(){
	LOG_DEBUG("sensorInitLib called");

    // get looper
    ALooper* looper = ALooper_forThread();
    if(looper == NULL)
        looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);

    // initialize event queue
    sensorEventQueue = ASensorManager_createEventQueue(sensorManager, looper, LOOPER_ID, getSensorEvents, sensor_data);

    // clear array of sensors
    for(int i = 0; i < NUMBER_OF_SENSORS; i++) sensors[i].sensor = NULL;

	LOG_DEBUG("sensorInitLib finished");
}

// Stop the library usage. Returns a negative error code on failure.
export int sensorStopLib(){
	LOG_DEBUG("sensorStopLib called");
    return ASensorManager_destroyEventQueue(sensorManager, sensorEventQueue);
}

// Start to use sensor of the specified type. Returns a negative error code on failure.
export int sensorUse(int type){
	LOG_DEBUG("sensorUse called");

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
	LOG_DEBUG("sensorSetEventRate called");
    return ASensorEventQueue_setEventRate(sensorEventQueue, sensors[type].sensor, eventRate);
}

// Disable a sensor of the specified type. Returns a negative error code on failure.
export int sensorDisable(int type){
	LOG_DEBUG("sensorDisable called");
    return ASensorEventQueue_disableSensor(sensorEventQueue, sensors[type].sensor);
}

// Enable a sensor of the specified type. Returns a negative error code on failure.
export int sensorEnable(int type){
	LOG_DEBUG("sensorEnable called");
    return ASensorEventQueue_enableSensor(sensorEventQueue, sensors[type].sensor);
}

/*
 * Get actual data for the selected scalar sensor type.
 * This is used for sensors such as ambient temperature,
 * proximity, light, pressure or humidity.
 */
export void sensorGetData1(int type, float &value){
	LOG_DEBUG("sensorGetData1 called");
    value = sensors[type].event.data[0];
}

/*
 * Get actual data for the selected 3D sensor type.
 * This is used for sensors such as accelerometer, magnetic field,
 * gyroscope, or gravity.
 */
export void sensorGetData3(int type, float &x, float &y, float &z){
	LOG_DEBUG("sensorGetData3 called");

    x = sensors[type].event.data[0];
    y = sensors[type].event.data[1];
    z = sensors[type].event.data[2];
}

/*
 * Get actual data for the selected 3D sensor type.
 * Rotation vector sensor uses <x, y, z, w> as components of
 * a unit quaternion representing rotation of the device.
 */
export void sensorGetData4(int type, float &x, float &y, float &z, float &w){
	LOG_DEBUG("sensorGetData4 called");

    x = sensors[type].event.data[0];
    y = sensors[type].event.data[1];
    z = sensors[type].event.data[2];
    w = sensors[type].event.data[3];
}

// Return name of a sensor of the specified type.
export const char* sensorGetName(int type){
	LOG_DEBUG("sensorGetName called");
	return ASensor_getName(sensors[type].sensor);
}

// Return vendor's name of a sensor of the specified type.
export const char* sensorGetVendor(int type){
	LOG_DEBUG("sensorGetVendor called");
	return ASensor_getVendor(sensors[type].sensor);
}

// Return resolution of a sensor of the specified type.
export float sensorGetResolution(int type){
	LOG_DEBUG("sensorGetResolution called");
	return ASensor_getResolution(sensors[type].sensor);
}

/*
 * Return the minimum delay allowed between events in microseconds for
 * a sensor of the specified type.
 */
export float sensorGetMinDelay(int type){
	LOG_DEBUG("sensorGetMinDelay called");
	return ASensor_getMinDelay(sensors[type].sensor);
}