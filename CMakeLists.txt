cmake_minimum_required(VERSION 3.9)
project(ChargeGuru)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake_modules/)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(SOURCES
  main.cpp
  mainwindow.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ChargeGuru ${SOURCES})
qt5_use_modules(ChargeGuru Core Gui Widgets Charts)

find_package(libusb-1.0)
if (LIBUSB_1_FOUND)
    include_directories(${LIBUSB_1_INCLUDE_DIRS})
    target_link_libraries(ChargeGuru ${LIBUSB_1_LIBRARIES})
endif(LIBUSB_1_FOUND)

target_link_libraries(ChargeGuru b6)

