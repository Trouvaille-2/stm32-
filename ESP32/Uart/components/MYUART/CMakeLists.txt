# 这是 ESP32 组件标准、最简、正确的 CMakeLists.txt
idf_component_register(
    SRCS "myuart.c"                # 只写一次源文件
    INCLUDE_DIRS "."            # 头文件当前目录（LED.h）
    REQUIRES driver             # 依赖驱动库
)

# 正确的编译优化选项
component_compile_options(
    -ffast-math 
    -O3                 # 大写 O，不是数字 0
    -Werror=format      # 正确拼写 W 不是 who
    -Wno-format
    -Wno-format-security
)