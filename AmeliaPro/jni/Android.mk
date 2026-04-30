LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := AnianLeakC17

LOCAL_CPPFLAGS += -std=c++20
LOCAL_CPPFLAGS += -Wno-error=format-security -Wno-error=c++11-narrowing -fpermissive
LOCAL_CPPFLAGS += -fexceptions -frtti -fvisibility=hidden -fvisibility-inlines-hidden
LOCAL_CPPFLAGS += -O2 -ffunction-sections -fdata-sections -fomit-frame-pointer
LOCAL_CPPFLAGS += -DMBEDTLS_THREADING_PTHREAD -DMBEDTLS_THREADING_C=0
LOCAL_CPPFLAGS += -fstrict-aliasing -funsafe-math-optimizations -ftree-vectorize -ffast-math
LOCAL_CPPFLAGS += -fstack-protector-strong -Wall -Wextra -Wno-format-security -Wno-narrowing -w -s
LOCAL_CPPFLAGS += -DANVM_ENABLED -DANVM_LEVEL_EXTREME -DANVM_VM_PROTECT
LOCAL_LDFLAGS += -Wl,--allow-multiple-definition
LOCAL_LDFLAGS += -Wl,-z,relro,-z,now -Wl,-z,noexecstack -Wl,--gc-sections
LOCAL_LDFLAGS += -Wl,--strip-all -Wl,--hash-style=gnu
LOCAL_LDFLAGS += -Wl,-z,max-page-size=4096
LOCAL_LDFLAGS += -Wl,-z,common-page-size=4096

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ImGui
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/My_Utils
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/vulkan
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/lib/freetype
LOCAL_C_INCLUDES += $(LOCAL_PATH)/lib/freetype/freetype
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/PhysX
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/ImGui/Fonts
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Hack
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src/验证系统
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Embree
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Embree/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/Embree/foundation

FILE_LIST := $(wildcard $(LOCAL_PATH)/src/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/PhysX/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/验证系统/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/ImGui/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/vulkan/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/My_Utils/*.cpp)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv1_CM -lGLESv2 -lGLESv3 -lm -lz -lvulkan
LOCAL_STATIC_LIBRARIES := embree txnet freetype

include $(BUILD_EXECUTABLE)

LOCAL_MODULE := embree
LOCAL_SRC_FILES := include/Embree/libembree4.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include/Embree
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := txnet
LOCAL_SRC_FILES := lib/libtxnet.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := freetype
LOCAL_SRC_FILES := lib/libfreetype.a
include $(PREBUILT_STATIC_LIBRARY)