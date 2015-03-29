LOCAL_PATH := $(call my-dir)/..

include $(CLEAR_VARS)

LOCAL_MODULE := ZgeSensor
LOCAL_SRC_FILES := ZgeSensor.cpp

LOCAL_LDLIBS += -landroid
#LOCAL_LDLIBS += -landroid-llog
LOCAL_CPPFLAGS += -fexceptions -frtti

include $(BUILD_SHARED_LIBRARY)
