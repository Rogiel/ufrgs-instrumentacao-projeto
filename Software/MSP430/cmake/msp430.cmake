include(CMakeForceCompiler)

#---------------------------------------------------------------------------------------------------------
# CMake Toolchain File for the MSP430
#
# Author: Matthew Iannucci
#
# NOTE: A lot of the paths here are hard coded. The reason is that this depends on the Energia App being installed on
# OSX which should have the same path on every computer. On Windows, it could be installed anywhere and its not
# worth the effort to find it rather than the user just writing it for themselves. Lastly, on Linux it assumes
# that all of the binaries are in /usr/bin and thus in the system path. It is assumed on linux the toolchain is
# installed by a package manager at /usr.
#
# - To make a 'flash' command, call the setup_flash_target function with the TARGET NAME and DRIVER of the launchpad
#   board.
#
#---------------------------------------------------------------------------------------------------------

# The name of the target operating system
set(CMAKE_SYSTEM_NAME Generic)
set(MSP_TOOLCHAIN 1)

# The location of the mspgcc toolchain. This may vary and need to be modified.
if(CMAKE_HOST_APPLE)
    set(MSP430_PATH /Applications/Energia.app/Contents/Resources/Java/hardware/tools/msp430)
elseif(CMAKE_HOST_WIN32)
    set(MSP430_PATH C:/Users/miannucci/msp430)
else()
    set(MSP430_PATH /usr/msp430)
endif()

# Specify the cross compiler
if(CMAKE_HOST_WIN32)
    CMAKE_FORCE_C_COMPILER(${MSP430_PATH}/bin/msp430-gcc.exe GNU)
    CMAKE_FORCE_CXX_COMPILER(${MSP430_PATH}/bin/msp430-gcc.exe GNU)
elseif(CMAKE_HOST_APPLE)
    CMAKE_FORCE_C_COMPILER(${MSP430_PATH}/bin/msp430-gcc GNU)
    CMAKE_FORCE_CXX_COMPILER(${MSP430_PATH}/bin/msp430-gcc GNU)
else()
    CMAKE_FORCE_C_COMPILER(msp430-gcc GNU)
    CMAKE_FORCE_CXX_COMPILER(msp430-gcc GNU)
endif()

# Where is the target environment located
set(CMAKE_FIND_ROOT_PATH ${MSP430_PATH})

# Adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Include The header directory
if (CMAKE_HOST_APPLE OR CMAKE_HOST_WIN32)
    include_directories(SYSTEM ${MSP430_PATH}/msp430/include)
else()
    include_directories(SYSTEM ${MSP430_PATH}/include)
endif()

# Create a function that will instantiate a flash target command using mspdebug.
# you may have to set the path to mspdebug if it is different
if (CMAKE_HOST_APPLE)
    set(MSPDEBUG_PATH ${MSP430_PATH}/mspdebug/mspdebug)
else()
    set(MSPDEBUG_PATH mspdebug)
endif()
function(setup_flash_target TARGET_NAME DRIVER)
    add_custom_target(flash
        COMMAND ${MSPDEBUG_PATH} ${DRIVER} 'prog ${TARGET_NAME}'
        DEPENDS ${TARGET_NAME}
    )
endfunction()