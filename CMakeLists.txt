cmake_minimum_required(VERSION 3.10)
project(lvgl)

set(LV_DRIVER "X11")

option(DEBUG_MODE "Disable optimizations and enable debugging" OFF)

set(CMAKE_C_STANDARD 99) # LVGL officially supports C99 and above
set(CMAKE_CXX_STANDARD 17) #C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (DEBUG_MODE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")
endif(DEBUG_MODE)

if (LV_DRIVER STREQUAL "SDL2")
    set(LV_DRIVER_USE "USE_SDL")
else()
    set(LV_DRIVER_USE "USE_${LV_DRIVER}")
endif()

add_definitions(-D${LV_DRIVER_USE}=1)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR})

add_subdirectory(lv_drivers)
target_include_directories(lv_drivers PUBLIC ${PROJECT_SOURCE_DIR})

# No specific build steps required for FBDEV
add_executable(lvgl-app main.c bacon.c bacon_theme.c )#btlib.c)
target_link_libraries(lvgl-app lv_drivers lvgl m pthread ${LV_DRIVER})
target_compile_definitions(lvgl-app PRIVATE SIMULATOR=1 LV_BUILD_TEST=0)

add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/lvgl-app DEPENDS lvgl-app)
