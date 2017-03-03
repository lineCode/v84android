cmake_minimum_required(VERSION 3.7)
project(v8_android)

# 打开编译输出
set(CMAKE_VERBOSE_MAKEFILE ON)
# 输出编译 target 名称，例如 Android、Linux、Darwin 等
message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

# 设置编程语言的版本
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)

# 删除不必要的导出符号
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")

# 如果是 Android
# 需要加入 -DANDROID 宏定义，和 ndk-build 保持相同
# 加入缺少的编译链接选项
if (CMAKE_SYSTEM_NAME STREQUAL Android)
    add_definitions(-DANDROID)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -MMD -MP -MF -ffunction-sections -fstack-protector -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -Wa,--noexecstack -Wformat -Werror=format-security")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined -pie -fPIE")
    # -Wl,--no-undefined
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined -no-canonical-prefixes -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -fno-rtti -fno-exceptions")
include_directories(src include)
link_directories(lib)

# 默认添加库代码
set(SOURCE_FILES
        src/main.cpp)

if (CMAKE_SYSTEM_NAME STREQUAL Android)
    # Android 平台上编译为一个 so 文件
#    add_library(node STATIC ${SOURCE_FILES})
    add_executable(v8_android ${SOURCE_FILES})
else ()
    # 非 Android 平台上编译为一个可执行程序
    add_executable(v8_android ${SOURCE_FILES})
endif ()

set(LINKED_LIBS ${LINKED_LIBS} v8)
set(LINKED_LIBS ${LINKED_LIBS} m dl)

if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    # Ubuntu 上需要加入线程的支持
    set(LINKED_LIBS ${LINKED_LIBS} pthread)
elseif (CMAKE_SYSTEM_NAME STREQUAL Android)
    # Android 上需要加入一些基本库
    set(LINKED_LIBS ${LINKED_LIBS} log)
endif ()

# 链接所有库
target_link_libraries(v8_android ${LINKED_LIBS})