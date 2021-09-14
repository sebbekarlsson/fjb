cmake_minimum_required(VERSION 3.20)


project(test)

file(GLOB sources src/*.c)

add_executable(test ${sources})


include_directories(${CMAKE_SOURCE_DIR}/src/include)

include(FetchContent)
macro (use_lib LIBNAME URL TAG REALNAME)
  if (${URL} MATCHES "git")
    FetchContent_Declare(
      ${LIBNAME}
      GIT_REPOSITORY ${URL}
      GIT_TAG ${TAG}
      )
  else()
    FetchContent_Declare(
      ${LIBNAME}
      ${URL}
      )
  endif()

  FetchContent_MakeAvailable(${LIBNAME})

  set(varname "${LIBNAME}_SOURCE_DIR")
  message(${CMAKE_BINARY_DIR}/_deps/include/${REALNAME})

  make_directory(${CMAKE_BINARY_DIR}/_deps/include/${REALNAME})
  make_directory(${CMAKE_BINARY_DIR}/_deps/include/${LIBNAME})

  file(GLOB g ${${varname}}/src/include/*)
  file(COPY ${g}  DESTINATION ${CMAKE_BINARY_DIR}/_deps/include/${REALNAME})
  include_directories(${CMAKE_BINARY_DIR}/_deps/include)
endmacro (use_lib)



use_lib(hashmap https://github.com/sebbekarlsson/hashmap.git master hashmap)
use_lib(json https://github.com/sebbekarlsson/libjson.git master json)
use_lib(scss https://github.com/sebbekarlsson/scss.git master scss)

make_directory(${CMAKE_BINARY_DIR}/_deps/include/fjb)
file(GLOB g ${CMAKE_SOURCE_DIR}/../src/include/*)
file(COPY ${g} DESTINATION ${CMAKE_BINARY_DIR}/_deps/include/fjb)

add_compile_options(dl pthread)

target_link_directories(test PUBLIC ${CMAKE_SOURCE_DIR}/../build)
target_link_libraries(test dl fjb_l pthread json scss hashmap)

file(COPY ${CMAKE_SOURCE_DIR}/fjb-samples DESTINATION ${CMAKE_BINARY_DIR})
