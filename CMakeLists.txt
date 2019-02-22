cmake_minimum_required(VERSION 2.6)
project(STENT C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -pedantic -Wall")

include_directories(
  src/stent
)

add_library(stent
  src/stent/stent.c
)

add_executable(emptest
  src/emptest/main.c
)
target_link_libraries(emptest stent)

add_executable(socket
  src/socket/main.c
  src/socket/socket.c
)
target_link_libraries(socket stent)
