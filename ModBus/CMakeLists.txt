cmake_minimum_required (VERSION 3.7.0) 
project(ups_service)
set(CMAKE_CXX_STANDARD 20)

target_sources(ups_service
PUBLIC
${CMAKE_CURRENT_LIST_DIR}/crc.cpp
${CMAKE_CURRENT_LIST_DIR}/ModbusManager.cpp
${CMAKE_CURRENT_LIST_DIR}/ModbusObject.cpp
${CMAKE_CURRENT_LIST_DIR}/SmartHandler.cpp
${CMAKE_CURRENT_LIST_DIR}/TdsComPort.cpp)



