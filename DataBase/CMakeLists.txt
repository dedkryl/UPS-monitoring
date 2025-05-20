cmake_minimum_required (VERSION 3.7.0) 
project(ups_service)
set(CMAKE_CXX_STANDARD 20)

target_sources(ups_service
PUBLIC
${CMAKE_CURRENT_LIST_DIR}/DBManager.cpp
${CMAKE_CURRENT_LIST_DIR}/sqlite3.c)



