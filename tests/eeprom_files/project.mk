PROJECT_NAME = eeprom_test

INC_PATH += $(LIBS_TESTS)mockFiles/inc
INC_PATH += $(LIBS_SUPPORT)memDrive/inc
#-------------------------------------------------------------------
SRC_FILES += $(LIBS_TESTS)mockFiles/src/api_EEPROM.c
SRC_FILES += $(LIBS_TESTS)mockFiles/src/api_RTC.c
SRC_FILES += $(LIBS_SUPPORT)memDrive/src/binaryFiles.c
SRC_FILES += $(MAKEFILE_DIR)src/main.c


CFLAGS += -DOS_NOT_VERBOSE=1
