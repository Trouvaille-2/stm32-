idf_component_register(
    SRCS "LED.c"
    INCLUDE_DIRS "."
    REQUIRES driver
)

component_compile_options(
    -ffast-math
    -O3
    -Werror=format
    -Wno-format
    -Wno-format-security
)