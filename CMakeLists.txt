cmake_minimum_required(VERSION 2.6)
project(STENT C)

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
