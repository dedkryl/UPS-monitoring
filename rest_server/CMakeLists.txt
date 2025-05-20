cmake_minimum_required (VERSION 3.7.0) 
project(ups_service)
set(CMAKE_CXX_STANDARD 20)


target_sources(ups_service
PUBLIC
${CMAKE_CURRENT_LIST_DIR}/json_builder.cpp
${CMAKE_CURRENT_LIST_DIR}/json_response_generator.cpp
${CMAKE_CURRENT_LIST_DIR}/rest_server.cpp
${CMAKE_CURRENT_LIST_DIR}/version.cpp
${CMAKE_CURRENT_LIST_DIR}/xml_builder.cpp
${CMAKE_CURRENT_LIST_DIR}/xml_response_generator.cpp)



