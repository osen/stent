cmake_minimum_required(VERSION 2.6)
project(STENT)

include_directories(
  stent
  posix
  mongoose
  sqlite3
)

add_library(stent
  stent/stent.c
)

add_library(posix
  posix/posix_s.c
)

add_library(mongoose
  mongoose/mongoose.c
  mongoose/mongoose_s.c
)

add_library(sqlite3
  sqlite3/sqlite3.c
)

add_executable(emptest
  emptest/main.c
  emptest/Employee.c
  emptest/Department.c
)

target_link_libraries(emptest stent)

