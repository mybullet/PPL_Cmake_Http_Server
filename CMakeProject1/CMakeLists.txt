# CMakeList.txt: CMakeProject1 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project (CMakeProject1)



# 设置Boost库的路径
set(Boost_INCLUDE_DIR "X:/boost_1_81_0/boost_1_81_0")
set(Boost_LIBRARY_DIR "X:/boost_1_81_0/boost_1_81_0/stage/lib")
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")


find_package(Boost REQUIRED COMPONENTS system filesystem thread regex)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libzmq/include)

# 添加Boost头文件路径
include_directories(${Boost_INCLUDE_DIR})
# Set Boost library directories
link_directories(${Boost_LIBRARY_DIRS})


# 添加 MySQL 的包含目录
include_directories("Y:/MySql/include")

# 添加 MySQL 的库目录
link_directories("Y:/MySql/lib")

# 将源代码添加到此项目的可执行文件。
add_executable (CMakeProject_exe  "CMakeProject1.cpp" "CMakeProject1.h" "HttpServer.h" "HttpServer.cpp" "ZmqSubscriber.h" "ZmqSubscriber.cpp"  "common.h" "common.cpp")
  

target_link_libraries(CMakeProject_exe
    ${Boost_LIBRARIES}
    ${CMAKE_CURRENT_SOURCE_DIR}/libzmq/lib/libzmq-v142-mt-gd-4_3_5.lib
    libmysql.lib
)

# TODO: 如有需要，请添加测试并安装目标。
