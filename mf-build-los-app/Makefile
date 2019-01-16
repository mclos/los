######################################
# target
######################################
TARGET = test_gpu
OPT = -O0
 
BUILD_DIR = build
 
C_SOURCES  = \
ugui.c \
test_gpu.c

# C defines  -DFREERTOS 
C_DEFS =   
 
 

C_INCLUDES		=  \
-I.
#######################################
# binaries
#######################################
PREFIX = 
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)clang.exe
AS = $(GCC_PATH)/$(PREFIX)gcc.exe -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
LD = $(GCC_PATH)/$(PREFIX)losld
else
CC = $(PREFIX)clang.exe
AS = $(PREFIX)clang.exe -x assembler-with-cpp
CP = $(PREFIX)objcopy
LD = $(PREFIX)losld
endif

CFLAGS = -std=c99 $(C_DEFS) $(C_INCLUDES) $(OPT)  -c -fmessage-length=0

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


# libraries
 

# default action: build all
all: $(TARGET).los

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

$(TARGET).los: $(OBJECTS) Makefile
	$(LD) $(OBJECTS) -o $@
	
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
