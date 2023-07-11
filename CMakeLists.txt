cmake_minimum_required(VERSION 3.23)
project(cn_project C)

set(CMAKE_C_STANDARD 99)
IF (WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexec-charset=GBK")
ENDIF ()
add_executable(receive receiver_main.c )
add_executable(send sender_main.c )
