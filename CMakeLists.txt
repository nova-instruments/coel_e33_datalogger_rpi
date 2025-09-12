cmake_minimum_required(VERSION 3.16)
project(modbus_reader C)

set(CMAKE_C_STANDARD 99)

# Configurar paths das dependências ARM
set(DEPS_DIR "${CMAKE_SOURCE_DIR}/deps")
set(LIBMODBUS_DIR "${DEPS_DIR}/libmodbus/install")
set(LIBGPIOD_DIR "${DEPS_DIR}/libgpiod/install")
set(LIBUDEV_DIR "${DEPS_DIR}/eudev/install")

# Verificar se as dependências existem (apenas warning, não erro fatal)
if(NOT EXISTS "${LIBMODBUS_DIR}/lib/libmodbus.so")
    message(WARNING "libmodbus não encontrada em ${LIBMODBUS_DIR}/lib/")
    message(WARNING "Execute: ./scripts/build_libmodbus_arm.sh")
endif()

if(NOT EXISTS "${LIBGPIOD_DIR}/lib/libgpiod.so")
    message(WARNING "libgpiod não encontrada em ${LIBGPIOD_DIR}/lib/")
    message(WARNING "Execute: ./scripts/build_libgpiod_arm.sh")
endif()

if(NOT EXISTS "${LIBUDEV_DIR}/lib/libudev.a")
    message(WARNING "libudev não encontrada em ${LIBUDEV_DIR}/lib/")
    message(WARNING "Execute: ./scripts/build_libudev_arm.sh")
endif()

# Include directories
include_directories(
    ${LIBMODBUS_DIR}/include
    ${LIBGPIOD_DIR}/include
    ${LIBUDEV_DIR}/include
)

# Link directories
link_directories(
    ${LIBMODBUS_DIR}/lib
    ${LIBGPIOD_DIR}/lib
    ${LIBUDEV_DIR}/lib
)

# Executável principal
add_executable(modbus_reader modbus_reader.c)

# Configurações de compilação
target_compile_options(modbus_reader PRIVATE
    -Wall
    -Wextra
    -O2
    -g
)

# Linking das bibliotecas
target_link_libraries(modbus_reader
    modbus
    gpiod
    udev
    pthread
)

# Configurar diretório de saída
set_target_properties(modbus_reader PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Informações de build
message(STATUS "=== Configuração do Projeto Modbus Reader ===")
message(STATUS "Compilador C: ${CMAKE_C_COMPILER}")
message(STATUS "Sistema alvo: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Cross-compiling: ${CMAKE_CROSSCOMPILING}")
message(STATUS "Diretório de dependências: ${DEPS_DIR}")
message(STATUS "Executável será gerado em: ${CMAKE_BINARY_DIR}/bin/modbus_reader")
message(STATUS "============================================")
