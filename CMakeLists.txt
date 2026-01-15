cmake_minimum_required(VERSION 3.30)

set(CMAKE_GENERATOR "MinGW Makefiles" CACHE STRING "" FORCE)

project(usagec LANGUAGES CXX)

add_executable(usagec usagec.cpp)

target_compile_features(usagec PRIVATE cxx_std_20)

if(WIN32)
    add_custom_command(
        TARGET usagec
        PRE_BUILD
        COMMAND taskkill /F /IM usagec.exe /T 2>nul || echo Process not running
        COMMENT "Terminating usagec.exe if it is running..."
    )
endif()