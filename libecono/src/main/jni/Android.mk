LOCAL_PATH := $(call my-dir)
LIBUSB_ROOT_REL:= ../..
LIBUSB_ROOT_ABS:= $(LOCAL_PATH)/../..
include $(CLEAR_VARS)

LOCAL_MODULE    := native
LOCAL_SRC_FILES := SerialPort.c


LOCAL_LDLIBS    := -llog
include $(BUILD_SHARED_LIBRARY)
