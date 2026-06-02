idf_component_register(
    # 按实际路径写所有源文件
    SRCS
        "LCD/lcd.c"
        "LED/LED.c"
        "SPI/spi.c"
        "ESPTimer/esptim.c"
    # 把所有子目录都加入头文件搜索路径
    INCLUDE_DIRS
        "."
        "LCD"
        "LED"
        "SPI"
        "ESPTimer"

    REQUIRES driver esp_timer
)

# 编译优化选项（可选）
component_compile_options(
    -O3
    -Wno-format
    -Wno-format-security
)