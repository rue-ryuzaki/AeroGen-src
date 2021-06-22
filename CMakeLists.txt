project(AeroGen)
cmake_minimum_required(VERSION 3.13)

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wold-style-cast -Wextra -Wundef -Wsign-compare -Wconversion -Wpointer-arith -Weffc++ -Wnon-virtual-dtor")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE PROJECT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(SRC_LIST ${PROJECT_HEADERS} ${PROJECT_SOURCES})

file(GLOB_RECURSE RESOURCES    "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc")
file(GLOB_RECURSE TRANSLATIONS "${CMAKE_CURRENT_SOURCE_DIR}/*.ts")

aux_source_directory(. SRC_LIST)

source_group("Resource Files" FILES ${RESOURCES})
source_group("Translation Files" FILES ${TRANSLATIONS})

add_executable(${PROJECT_NAME} ${SRC_LIST} ${RESOURCES} ${TRANSLATIONS})

#find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets OpenGL)
#target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network OpenGL)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::OpenGL)

if(WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive)
    if(MSVC)
        target_link_libraries(${PROJECT_NAME} opengl32.lib glu32.lib)
    else()
        target_link_libraries(${PROJECT_NAME} -lopengl32 -lglu32)
    endif()
elseif(APPLE)
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})
else()
    target_link_libraries(${PROJECT_NAME} -lGL -lGLU)
endif()

if(NOT WIN32)
    target_link_libraries(${PROJECT_NAME} -lpthread)
endif()

# Qwt
find_path(QWT_INCLUDE_DIR
    NAMES qwt_plot.h
    HINTS ${QT_INCLUDE_DIR}
    PATH_SUFFIXES qwt qwt-qt3 qwt-qt4 qwt-qt5
)
find_library(QWT_LIBRARY
    NAMES qwt qwt-qt3 qwt-qt4 qwt-qt5
)
if(QWT_INCLUDE_DIR AND QWT_LIBRARY)
    add_definitions(-DQWT_DEFINED)
    include_directories(${QWT_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${QWT_LIBRARY})
endif()
