cmake_minimum_required(VERSION 3.0)
project(STENT C)

add_definitions(-D_XOPEN_SOURCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pedantic -Wall -Werror")

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
create_test(fstream true)

create_test(vec true)

create_test(dangling_ref false)
create_test(dangling_ref_copy false)
create_test(invalid_cast false)
create_test(invalid_void_cast false)
create_test(use_null false)
create_test(release_null false)
create_test(leak false)
create_test(vector_oob false)
create_test(vector_use_null false)
create_test(vector_dangling_ref false)
# vector_leak
# vector_incorrect_delete
