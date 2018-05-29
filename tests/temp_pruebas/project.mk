PROJECT_NAME = test_temperatura

INC_PATH += $(LIBS_SUPPORT)dspMath/inc
INC_PATH += $(LIBS_PATH)lpc4337_m4/chip/inc
#-------------------------------------------------------------------
SRC_FILES += $(LIBS_SUPPORT)dspMath/src/temperaturaMath.c
SRC_FILES += $(MAKEFILE_DIR)src/main.c


CFLAGS += -DOS_NOT_VERBOSE=1
