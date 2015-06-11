cmake_minimum_required( VERSION 2.8 )

project( signal_logger )

# set C++11 flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra")

# add to module path, so we can find our cmake modules
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules ${CMAKE_MODULE_PATH} )


find_package( ZeroMQ REQUIRED )
find_package( ZeroMQPP REQUIRED )
find_package( HAL REQUIRED )
find_package( Node REQUIRED )
#find_package( MINIGLOG REQUIRED )

include_directories( ${ZeroMQ_INCLUDE_DIRS} )
include_directories( ${ZeroMQPP_INCLUDE_DIRS} )
include_directories( ${HAL_INCLUDE_DIRS} )
include_directories( ${Node_INCLUDE_DIRS} )
#include_directories( ${MINIGLOG_INCLUDE_DIRS} )



set( HDRS FtdiDriver.h)
set( SRCS signal_logger.cpp)

add_executable( ${PROJECT_NAME} ${HDRS} ${SRCS} )
target_link_libraries( ${PROJECT_NAME}
    ${ZeroMQ_LIBRARIES}
    ${ZeroMQPP_LIBRARIES}
    ${HAL_LIBRARIES}
    ${Node_LIBRARIES} 
#    ${MINIGLOG_LIBRARIES} 
    )
