set(CMAKE_SYSTEM_NAME Generic)
set(cmake_system_processor arm)
set(cmake_crosscompiling 1)
set(DEVKITA64 $ENV{DEVKITA64})
set(DEVKITPRO $ENV{DEVKITPRO})
set(CMAKE_C_COMPILER ${DEVKITA64}/bin/aarch64-none-elf-gcc)
set(CMAKE_CXX_COMPILER ${DEVKITA64}/bin/aarch64-none-elf-g++)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-Wall -specs=${DEVKITPRO}/libnx/switch.specs")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "-Wall -specs=${DEVKITPRO}/libnx/switch.specs")
set(CMAKE_C_FLAGS "")

#ARCH    :=  -march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE
#
#CFLAGS  :=  -g -Wall -O2 -ffunction-sections \
#            $(ARCH) $(DEFINES)
#
#CFLAGS  +=  $(INCLUDE) -D__SWITCH__
#
#CXXFLAGS    := $(CFLAGS) -fno-rtti -fno-exceptions
#
#ASFLAGS :=  -g $(ARCH)
#LDFLAGS =   -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)
#
#LIBS    := -lnx -lm

set(CMAKE_CXX_FLAGS "")
function(add_switch_nro NRONAME)
    set(NRO_SRC ${ARGV})
    list(REMOVE_AT NRO_SRC 0)
    add_executable(${NRONAME}.elf ${NRO_SRC})
    add_custom_target(${NRONAME}.nro ALL
        COMMAND ${DEVKITPRO}/tools/bin/elf2nro ${NRONAME}.elf ${NRONAME}.nro
                    DEPENDS ${NRONAME}.elf
                    COMMENT "Creating NRO executable ${NRONAME}.nro")
    add_dependencies(${NRONAME}.nro ${NRONAME}.elf)
    set_directory_properties(PROPERTIES  ADDITIONAL_MAKE_CLEAN_FILES ${NRONAME}.nro)
endfunction(add_switch_nro)
