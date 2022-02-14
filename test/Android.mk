LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
 
LOCAL_SRC_FILES:= \
	test.cpp
 
LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
    libui \
    libgui \
	libbinder \
	libskia

LOCAL_C_INCLUDES += \
	external/skia/include/core \
	external/skia/include/utils 

LOCAL_MODULE:= SurfaceTest
 
LOCAL_MODULE_TAGS := tests
 
include $(BUILD_EXECUTABLE)
 