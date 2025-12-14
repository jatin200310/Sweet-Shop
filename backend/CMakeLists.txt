cmake_minimum_required(VERSION 3.10)
project(sweet_shop_backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- vcpkg libraries ----
find_package(Crow CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

# MySQL (manual)
set(MYSQL_INCLUDE_DIR "C:/Program Files/MySQL/MySQL Server 8.0/include")
set(MYSQL_LIBRARY "C:/Program Files/MySQL/MySQL Server 8.0/lib/libmysql.lib")

include_directories(
    ${MYSQL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(SOURCES
    src/main.cpp
    src/Database.cpp
    src/Auth.cpp
    src/Sweet.cpp
    src/JWT.cpp
)

add_executable(sweet_shop ${SOURCES})

target_link_libraries(sweet_shop
    PRIVATE
        Crow::Crow
        OpenSSL::SSL
        OpenSSL::Crypto
        ${MYSQL_LIBRARY}
)

set_target_properties(sweet_shop PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
