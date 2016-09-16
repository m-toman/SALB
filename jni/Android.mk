LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS    := -DNOFLITE
LOCAL_CFLAGS    += -D__GXX_EXPERIMENTAL_CXX0X__

FILE_LIST := $(wildcard $(LOCAL_PATH)/*.c*)
FILE_LIST := $(wildcard $(LOCAL_PATH)/../engine/manager/src/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../engine/synthesis/hts_engine/**/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../engine/text/internal/src/*.c*)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../engine/manager/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../engine/synthesis/hts_engine/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../engine/text/internal/include

LOCAL_MODULE    := SALB
LOCAL_LDLIBS := -llog


include $(BUILD_SHARED_LIBRARY)
