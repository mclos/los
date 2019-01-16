######################################
# target
######################################
TARGET = los
OPT = -O3
P = 1
BUILD_DIR = build
 
C_SOURCES  = \
arch/pc_ini/pc_ini.c  \
arch/win/los_fat.c  \
arch/win/lcdDriver.c  \
arch/win/los_audio.c  \
arch/win/los_gpu.c  \
arch/win/los_usart.c  \
arch/win/los_ram.c  \
arch/win/los_time.c  \
arch/win/user_io.c  \
arch/win/win32-losgui.c  \
arch/win/win32_uart.c  \
los-code/los.c  \
los-code/los_fat_config.c  

# C defines  -DFREERTOS 
C_DEFS =  \
-DFAT_CAPI \
-DWINDOW  
 

ifeq ($(P),0)
C_DEFS += -DLP_LOG_LOCAL_LEVEL=LP_LOG_NONE 
else ifeq ($(P),1)
C_DEFS += -DLP_LOG_LOCAL_LEVEL=LP_LOG_ERROR 
else ifeq ($(P),2)
C_DEFS += -DLP_LOG_LOCAL_LEVEL=LP_LOG_WARN 
else ifeq ($(P),3)
C_DEFS += -DLP_LOG_LOCAL_LEVEL=LP_LOG_DEBUG 
else ifeq ($(P),4)
C_DEFS += -DLP_LOG_LOCAL_LEVEL=LP_LOG_ALL 
else ifeq ($(P),5)
endif

C_INCLUDES		=  \
-Iarch  \
-Iarch/pc_ini  \
-Iarch/win  \
-Iinc  \
-Ilos-code
#######################################
# binaries
#######################################
PREFIX = 
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc.exe
AS = $(GCC_PATH)/$(PREFIX)gcc.exe -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc.exe
AS = $(PREFIX)gcc.exe -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif

CFLAGS = -std=c99 $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections -c -fmessage-length=0

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


# libraries
LIBS = -lgdi32 -lWINMM
LIBDIR = 
LDFLAGS =   $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(TARGET).exe

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(TARGET).exe: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@
	@echo P=0[no]/1[err]/2[warn]/3[debug]/4[info]*/5[all][printf level]
	@echo printf level $(P),build type $(TOUCH).
	
$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
