include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/include)

set( TH_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set(TH_CFLAGS -w -B ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} -I. -I${CMAKE_SOURCE_DIR}/include)
if(WIN32)
  set(TH_CFLAGS ${TH_CFLAGS} -I${CMAKE_SOURCE_DIR}/win32/include -I${CMAKE_SOURCE_DIR}/win32/include/winapi)
  set(EXESUF .exe )
  set(LIBTCC_LDFLAGS -Wl,-rpath=${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else()
  set(TCC_MATH_LDFLAGS )
  set(LIBTCC_LDFLAGS -Wl,-rpath=${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

file(GLOB TCC_TESTS *.c)
list(REMOVE_ITEM TCC_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/testmain.c)
file(MAKE_DIRECTORY  ${CMAKE_BINARY_DIR}/cc)
foreach(test_file ${TCC_TESTS})
    get_filename_component(test_file_NOEXT ${test_file} NAME_WE)
    add_custom_target(
        ${test_file_NOEXT} ALL
        COMMAND tcc ${CMAKE_CURRENT_SOURCE_DIR}/${test_file_NOEXT}.c ${CMAKE_CURRENT_SOURCE_DIR}/testmain.c ${TH_CFLAGS} -o ${CMAKE_BINARY_DIR}/cc/${test_file_NOEXT}${EXESUF}
        BYPRODUCTS ${CMAKE_BINARY_DIR}/cc/${test_file_NOEXT}${EXESUF}
        SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${test_file_NOEXT}.c )
    add_test(NAME ${test_file_NOEXT} COMMAND ${test_file_NOEXT} WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/cc)
endforeach()
