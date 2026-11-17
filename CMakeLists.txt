
cmake_minimum_required(VERSION 3.21)

project(BotDC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar DPP instalado mediante vcpkg
find_package(dpp CONFIG REQUIRED)

# Crear el ejecutable
add_executable(bot
    src/s/main.cpp
)

# Enlazar DPP
target_link_libraries(bot PRIVATE dpp::dpp)
