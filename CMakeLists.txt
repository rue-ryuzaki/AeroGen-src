project(AeroGen)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wold-style-cast -Wextra -Wundef -Wsign-compare -Wconversion -Wpointer-arith -Weffc++ -Wnon-virtual-dtor")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")
#-DCMAKE_BUILD_TYPE=Debug
#-DCMAKE_BUILD_TYPE=Release
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

cmake_policy(SET CMP0015 NEW)

file(GLOB_RECURSE PROJECT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(SRC_LIST
# project
    ${PROJECT_HEADERS}
    ${PROJECT_SOURCES}
)

file(GLOB_RECURSE RESOURCES    "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc")
file(GLOB_RECURSE TRANSLATIONS "${CMAKE_CURRENT_SOURCE_DIR}/*.ts")

aux_source_directory(. SRC_LIST)
add_definitions(-DQWT_DEFINED)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${RESOURCES} ${TRANSLATIONS})

# Qt5Core
find_package(Qt5Core)
if(Qt5Core_FOUND)
    include_directories(${Qt5Core_INCLUDE_DIRS})
    link_directories(${Qt5Core_LIBRARIES})
    add_definitions(${Qt5Core_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
    target_link_libraries(${PROJECT_NAME} ${Qt5Core_LIBRARIES})
endif(Qt5Core_FOUND)
# Qt5Gui
find_package(Qt5Gui)
if(Qt5Gui_FOUND)
    include_directories(${Qt5Gui_INCLUDE_DIRS})
    link_directories(${Qt5Guie_LIBRARIES})
    add_definitions(${Qt5Gui_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}")
    target_link_libraries(${PROJECT_NAME} ${Qt5Gui_LIBRARIES})
endif(Qt5Gui_FOUND)
# Qt5Widgets
find_package(Qt5Widgets)
if(Qt5Widgets_FOUND)
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    link_directories(${Qt5Widgets_LIBRARIES})
    add_definitions(${Qt5Widgets_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
    target_link_libraries(${PROJECT_NAME} ${Qt5Widgets_LIBRARIES})
endif(Qt5Widgets_FOUND)
# Qt5Network
find_package(Qt5Network)
if(Qt5Network_FOUND)
    include_directories(${Qt5Network_INCLUDE_DIRS})
    link_directories(${Qt5Network_LIBRARIES})
    add_definitions(${Qt5Network_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Network_EXECUTABLE_COMPILE_FLAGS}")
    target_link_libraries(${PROJECT_NAME} ${Qt5Network_LIBRARIES})
endif(Qt5Network_FOUND)
# Qt5OpenGL
find_package(Qt5OpenGL)
if(Qt5OpenGL_FOUND)
    set(QT_USE_QTOPENGL TRUE)
    include_directories(${Qt5OpenGL_INCLUDE_DIRS})
    link_directories(${Qt5OpenGL_LIBRARIES})
    add_definitions(${Qt5OpenGL_DEFINITIONS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS}")
    target_link_libraries(${PROJECT_NAME} ${Qt5OpenGL_LIBRARIES})
endif(Qt5OpenGL_FOUND)
# Qwt
find_path(QWT_INCLUDE_DIR
    NAMES qwt_plot.h
    HINTS ${QT_INCLUDE_DIR}
    PATH_SUFFIXES qwt qwt-qt3 qwt-qt4 qwt-qt5
)
include_directories(${QWT_INCLUDE_DIR})
find_library(QWT_LIBRARY
    NAMES qwt qwt-qt3 qwt-qt4 qwt-qt5
)

target_link_libraries(${PROJECT_NAME} ${QWT_LIBRARY} -lpthread -lGL -lGLU)
