cmake_minimum_required(VERSION 3.20)


project(fjb)


FILE(GLOB sources src/*.c)
FILE(GLOB sources_enums src/enums/*.c)
FILE(GLOB sources_hooks src/hooks/*.c)
FILE(GLOB sources_js src/js/*.c)

add_executable(fjb ${sources} ${sources_enums} ${sources_hooks} ${sources_src})

add_library(fjb_l ${sources} ${sources_enums} ${sources_hooks} ${sources_src})

include(FetchContent)


function (use_lib LIBNAME URL TAG)
  FetchContent_Declare(
    ${LIBNAME}
    GIT_REPOSITORY ${URL}
    GIT_TAG ${TAG}
  )
  FetchContent_MakeAvailable(${LIBNAME})

  set(varname "${LIBNAME}_SOURCE_DIR")
  make_directory(${CMAKE_BINARY_DIR}/_deps/include/${LIBNAME})
  file(GLOB g ${${varname}}/src/include/*)
  file(COPY ${g}  DESTINATION ${CMAKE_BINARY_DIR}/_deps/include/${LIBNAME})
  include_directories(${CMAKE_BINARY_DIR}/_deps/include)
endfunction (use_lib)


use_lib(hashmap https://github.com/sebbekarlsson/hashmap.git master)
use_lib(json https://github.com/sebbekarlsson/libjson.git master)
use_lib(scss https://github.com/sebbekarlsson/scss.git master)

add_compile_options(dl pthread)


target_link_libraries(fjb dl pthread json scss hashmap)
target_link_libraries(fjb_l dl pthread json scss hashmap)
