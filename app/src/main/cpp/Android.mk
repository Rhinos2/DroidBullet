LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE   := testBullet
LOCAL_CFLAGS   := -O3 -mno-thumb 
LOCAL_CPPFLAGS := $(LOCAL_CFLAGS)

SRC_FILES := $(wildcard $(LOCAL_PATH)/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../framework/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../framework/bullet/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../framework/ttf/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../framework/nvtristrip/*.cpp)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../framework/png/*.c)
SRC_FILES += $(wildcard $(LOCAL_PATH)/../framework/zlib/*.c)
SRC_FILES := $(SRC_FILES:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES := $(SRC_FILES)
LOCAL_LDLIBS += -lz -lm -ldl -lGLESv3 -lEGL -llog
LOCAL_CFLAGS := -O3 -mno-thumb -Wno-write-strings
LOCAL_CPPFLAGS := -O3 -mno-thumb -Wno-write-strings
include $(BUILD_SHARED_LIBRARY)
