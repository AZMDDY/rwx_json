# CMake最低版本要求
cmake_minimum_required(VERSION 3.1.0)

# C/C++标准： C11/CXX14
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

# cmake编译类型
set(CMAKE_BUILD_TYPE Release)

# 项目类型: 
# EXE: 可执行文件
# STATIC_LIB: 静态库
# SHARED_LIB: 动态库
set(PROJECT_TYPE "EXE")

# 设置项目名称
set(PROJECT_NAME rwxjson)

# 项目名称
project(${PROJECT_NAME})

# 可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out/bin)
# 库文件输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/out/lib)

# 添加子目录(子目录中有CMakeLists.txt文件)
# add_subdirectory()

# 头文件路径
include_directories(${PROJECT_SOURCE_DIR})

# 库文件路径
#link_directories()

# 源文件
file(GLOB_RECURSE SOURCES
    "rwx_json.cpp"
)

if(${PROJECT_TYPE} STREQUAL "EXE")
# 指定编译成可执行文件
add_executable(${PROJECT_NAME} main.cpp ${SOURCES})
# 链接所使用到的库
# target_link_libraries(${PROJECT_NAME} pthread)
elseif(${PROJECT_TYPE} STREQUAL "SHARED_LIB")
add_library(${PROJECT_NAME} SHARED ${SOURCES})
# 链接所使用到的库
# target_link_libraries(${PROJECT_NAME} pthread)
else()
add_library(${PROJECT_NAME} STATIC ${SOURCES})
# 链接所使用到的库
# target_link_libraries(${PROJECT_NAME} pthread)
endif()












