cmake_minimum_required(VERSION 2.6)
project(STENT C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c89 -pedantic -Wall -Werror")

include_directories(
  include
)

function(create_test NAME RETURN_ZERO)
  add_executable(${NAME} src/tests/${NAME}.c)

  if(RETURN_ZERO)
    add_test(${NAME} ./${NAME})
  else()
    add_test(${NAME} harness ./${NAME})
  endif()

endfunction()

add_executable(harness
  src/harness/main.c
)

enable_testing()

create_test(ref true)
create_test(ref_copy true)
create_test(copy true)
create_test(zero_initialized true)
create_test(void_cast true)
create_test(vector true)

create_test(dangling_ref false)
create_test(dangling_ref_copy false)
create_test(invalid_cast false)
create_test(invalid_void_cast false)
create_test(use_null false)
create_test(release_null false)
create_test(leak false)

