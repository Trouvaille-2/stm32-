# 这是 ESP32 组件标准、最简、正确的 CMakeLists.txt
idf_component_register(
    SRCS "beep.c"                # 只写一次源文件
    INCLUDE_DIRS "."            # 头文件当前目录
    REQUIRES driver             # 依赖驱动库
)
