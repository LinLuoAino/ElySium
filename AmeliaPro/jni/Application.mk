APP_ABI := arm64-v8a
APP_PLATFORM := android-29
APP_STL := c++_static
APP_ABI := arm64-v8a
APP_CFLAGS += -D_GNU_SOURCE
APP_LDFLAGS += -Wl,-z,max-page-size=0x1000
APP_CPPFLAGS += -fexceptions