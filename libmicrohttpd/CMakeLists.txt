cmake_minimum_required (VERSION 3.7.0) 
project(ups_service)
set(CMAKE_CXX_STANDARD 20)

#https://stackoverflow.com/questions/1620918/cmake-and-libpthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#target_link_libraries(ups_service PRIVATE Threads::Threads)

target_sources(ups_service
PUBLIC
${CMAKE_CURRENT_LIST_DIR}/connection.cpp
${CMAKE_CURRENT_LIST_DIR}/daemon.cpp
${CMAKE_CURRENT_LIST_DIR}/internal.cpp
${CMAKE_CURRENT_LIST_DIR}/memorypool.cpp
${CMAKE_CURRENT_LIST_DIR}/postprocessor.cpp
${CMAKE_CURRENT_LIST_DIR}/reason_phrase.cpp
${CMAKE_CURRENT_LIST_DIR}/response.cpp
${CMAKE_CURRENT_LIST_DIR}/w32functions.cpp)
