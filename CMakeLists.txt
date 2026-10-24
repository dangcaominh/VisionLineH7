cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

file(GLOB_RECURSE OV5640_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Drivers/OV5640/*.c")
file(GLOB_RECURSE ITR20001_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Drivers/ITR20001/*.c")
file(GLOB_RECURSE Encoder_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Drivers/Encoder/*.c")
file(GLOB_RECURSE TB6612FNG_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Drivers/TB6612FNG/*.c")
file(GLOB_RECURSE Camera_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Drivers/Camera/*.c")
file(GLOB_RECURSE USB_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Drivers/USB/*.c")
file(GLOB_RECURSE APPLICATION_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/Application/Src/*.c")


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME VisionLineH7)


# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${OV5640_SOURCES}
    ${USB_COMPOSITE_SOURCES}
    ${ITR20001_SOURCES}
    ${Encoder_SOURCES}
    ${TB6612FNG_SOURCES}
    ${Camera_SOURCES}
    ${USB_SOURCES}
    ${APPLICATION_SOURCES}
    tinyusb/src/class/cdc/cdc_device.c
    tinyusb/src/class/video/video_device.c
    tinyusb/src/tusb.c
    tinyusb/src/device/usbd.c
    tinyusb/src/common/tusb_fifo.c
    tinyusb/src/portable/synopsys/dwc2/dcd_dwc2.c
    tinyusb/src/portable/synopsys/dwc2/dwc2_common.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    Drivers/OV5640
    Drivers/ITR20001
    Drivers/Encoder
    Drivers/TB6612FNG
    Drivers/Camera
    Drivers/USB
    tinyusb/src
    tinyusb/src/common
    tinyusb/src/portable/synopsys/dwc2
    Application/Inc
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
