CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(ups_service)
set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")# C++11 support on GCC

add_executable(ups_service main.cpp ups_service.cpp HandledData.cpp utils.cpp)
add_subdirectory(ModBus)
add_subdirectory(rest_server)
add_subdirectory(libmicrohttpd)
add_subdirectory(DataBase)

#https://stackoverflow.com/questions/1620918/cmake-and-libpthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
#-ldl for dlopen() , dlclose() in sqlite
#https://stackoverflow.com/questions/956640/linux-c-error-undefined-reference-to-dlopen
target_link_libraries(ups_service PRIVATE Threads::Threads ${CMAKE_DL_LIBS})
