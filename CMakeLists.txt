cmake_minimum_required(VERSION 2.8.4)
project(NixView CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -std=c++11 -Wall") ## Optimize
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wunreachable-code")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -ggdb3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

########################################
# Qt5

find_package(Qt5Core QUIET)
find_package(Qt5Widgets QUIET)
find_package(Qt5PrintSupport QUIET)

include_directories(BEFORE
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5PrintSupport_INCLUDE_DIRS})

add_definitions(${Qt5Core_DEFINITIONS})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(NIX REQUIRED)
include_directories(AFTER ${NIX_INCLUDE_DIR})

########################################
# Boost
if(WIN32)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
else()
  set(Boost_USE_STATIC_LIBS OFF)
endif()

set(Boost_USE_MULTITHREADED ON)

find_package(Boost 1.49.0 REQUIRED date_time regex program_options system filesystem)

include_directories(AFTER ${Boost_INCLUDE_DIR})

########################################
# Configure the target
include_directories(${CMAKE_BINARY_DIR})

file(GLOB NIXVIEW_SOURCES *.cpp 
  infowidget/*.cpp 
  plotter/*.cpp 
  model/*.cpp 
  views/*.cpp 
  filter/*.cpp 
  options/*.cpp
  options/viewoptions/*.cpp)

file(GLOB NIXVIEW_INCLUDES *.hpp 
  infowidget/*.hpp 
  plotter/*.h 
  model/*.hpp 
  views/*.hpp 
  filter/*.hpp 
  options/*.hpp
  options/viewoptions/*.hpp)

file(GLOB NIXVIEW_UI *.ui 
  infowidget/*.ui 
  plotter/*.ui 
  views/*.ui 
  options/.*ui
  options/viewoptions/*.ui)

file(GLOB NIXVIEW_RES *.qrc 
  infowidget/*.qrc)

QT5_WRAP_UI(NIXVIEW_UI_HDRS ${NIXVIEW_UI})
QT5_ADD_RESOURCES(NIXVIEW_RES_SOURCES ${NIXVIEW_RES})

add_executable(nix-view ${NIXVIEW_UI_HDRS} ${NIXVIEW_INCUDES} ${NIXVIEW_SOURCES} ${NIXVIEW_RES_SOURCES})
target_link_libraries(nix-view Qt5::Core Qt5::Widgets Qt5::PrintSupport ${NIX_LIBRARIES} ${Boost_LIBRARIES})
qt5_use_modules(nix-view Gui)

########################################
# Status
get_property(INC_DIRS DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
MESSAGE("===============================")
MESSAGE("Binary dir: ${CMAKE_BINARY_DIR}")
MESSAGE("Configuration:")
