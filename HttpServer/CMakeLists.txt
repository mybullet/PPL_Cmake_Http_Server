
cmake_minimum_required (VERSION 3.8)

project (HttpServer)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES CMakeProject1.cpp CMakeProject1.h HttpServer.h HttpServer.cpp ZmqSubscriber.h ZmqSubscriber.cpp  common.h common.cpp)

find_package(Boost REQUIRED COMPONENTS system filesystem thread regex chrono)


include_directories(/usr/include/mysql)

add_executable(${PROJECT_NAME} ${SOURCES})


target_link_libraries(${PROJECT_NAME}
    Boost::system Boost::thread Boost::filesystem Boost::regex Boost::chrono
    /usr/lib/x86_64-linux-gnu/libmysqlclient.so
    zmq
    pthread
    stdc++
    )


set(CMAKE_INSTALL_PREFIX /usr/local)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)


