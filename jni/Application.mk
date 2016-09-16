APP_PLATFORM := android-7
#APP_PLATFORM := android-14 # prefer this if possible
APP_ABI := all

#commandline: NDK_DEBUG=1 APP_OPTIM=debug
#NDK_TOOLCHAIN_VERSION := 4.8
#NDK_DEBUG := 1
#APP_OPTIM := debug 

APP_STL := gnustl_static
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -std=gnu++11
#APP_CPPFLAGS += -std=c++11
