LOCAL_PATH := $(call my-dir)/..

include $(CLEAR_VARS)

LOCAL_MODULE := ZGESensor
LOCAL_SRC_FILES := ZGESensor.cpp

LOCAL_LDLIBS += -landroid

include $(BUILD_SHARED_LIBRARY)
