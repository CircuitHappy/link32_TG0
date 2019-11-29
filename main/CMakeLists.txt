idf_component_register(SRCS main.cpp gpio.cpp)
target_compile_options(${COMPONENT_LIB} PRIVATE -fexceptions)
