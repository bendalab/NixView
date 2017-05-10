message ("-------------------------------------------")
message ("-       setting up nixview build          -")
message ("-------------------------------------------")

cmake_minimum_required (VERSION 2.8.11)

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE DEBUG)
endif ()

message ("=> ${CMAKE_BUILD_TYPE} configuration")

project (NixView CXX)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_INIT} -std=c++11 -Wall") ## Optimize
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wunreachable-code")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -ggdb3")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include (NixView) # project settings
include (QtCommon) # qt settings
include (CMakeCPackOptions) # packaging config
include (CPack)

fix_project_version ()
add_project_meta (META_FILES_TO_INCLUDE)

########################################
# Qt5
message ("=> finding qt modules ...")
find_package (Qt5Core REQUIRED)
find_package (Qt5Widgets REQUIRED)
find_package (Qt5PrintSupport QUIET)
find_package (Qt5Sql REQUIRED)

include_directories (BEFORE
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5PrintSupport_INCLUDE_DIRS}
  ${Qt5Sql_INCLUDE_DIRS})

add_definitions (${Qt5Core_DEFINITIONS})

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)

find_package (NIX REQUIRED)
include_directories (AFTER ${NIX_INCLUDE_DIR})

########################################
# Boost
if (WIN32)
  set (Boost_USE_STATIC_LIBS ON)
  set (Boost_USE_STATIC_RUNTIME OFF)
else ()
  if (CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    set (Boost_USE_STATIC_LIBS OFF)
  else ()
    set (Boost_USE_STATIC_LIBS ON)
  endif ()
endif()

set (Boost_USE_MULTITHREADED ON)

find_package (Boost 1.49.0 REQUIRED date_time regex program_options system filesystem)

include_directories (AFTER ${Boost_INCLUDE_DIR})

########################################
# Configure the target
include_directories (${CMAKE_BINARY_DIR})

file (GLOB NIXVIEW_SOURCES *.cpp 
  dialogs/*.cpp
  infowidget/*.cpp 
  plotter/*.cpp 
  model/*.cpp 
  views/*.cpp 
  filter/*.cpp 
  options/*.cpp
  utils/*.cpp
  db/*.cpp)

file (GLOB NIXVIEW_INCLUDES *.hpp 
  *.h
  dialogs/*.hpp
  infowidget/*.hpp 
  plotter/*.h 
  model/*.hpp 
  views/*.hpp 
  filter/*.hpp 
  options/*.hpp
  db/*.hpp)

file (GLOB NIXVIEW_UI *.ui
  dialogs/*.ui
  infowidget/*.ui 
  plotter/*.ui 
  views/*.ui 
  options/*.ui)

file (GLOB NIXVIEW_RES *.qrc 
  infowidget/*.qrc)

QT5_WRAP_UI (NIXVIEW_UI_HDRS ${NIXVIEW_UI})
QT5_ADD_RESOURCES (NIXVIEW_RES_SOURCES ${NIXVIEW_RES})

if (CMAKE_BUILD_TYPE STREQUAL "RELEASE")
  add_executable (nixview ${OS_BUNDLE} ${NIXVIEW_UI_HDRS} ${NIXVIEW_INCUDES}
    ${NIXVIEW_SOURCES} ${NIXVIEW_RES_SOURCES} ${META_FILES_TO_INCLUDE})
else ()
  add_executable (nixview ${NIXVIEW_UI_HDRS} ${NIXVIEW_INCUDES}
    ${NIXVIEW_SOURCES} ${NIXVIEW_RES_SOURCES} ${META_FILES_TO_INCLUDE})
endif ()

target_link_libraries (nixview Qt5::Core Qt5::Widgets Qt5::PrintSupport Qt5::Sql
  ${NIX_LIBRARIES} ${Boost_LIBRARIES})

install (TARGETS nixview BUNDLE DESTINATION . RUNTIME  DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")

########################################
# Status
message ("Binary dir: ${CMAKE_BINARY_DIR}")


