include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/include)

set( TH_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set(TH_CFLAGS -w -B ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} -I${CMAKE_SOURCE_DIR}/include)
if(WIN32)
  set(TH_CFLAGS ${TH_CFLAGS} -I${CMAKE_SOURCE_DIR}/win32/include -I${CMAKE_SOURCE_DIR}/win32/include/winapi)
  set(EXESUF .exe )
  set(LIBTCC_LDFLAGS -Wl,-rpath=${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else()
  set(TCC_MATH_LDFLAGS )
  set(LIBTCC_LDFLAGS -Wl,-rpath=${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

add_custom_target(
    test-cthread ALL
    COMMAND tcc ${TH_DIR}/test-cthread.c ${TH_CFLAGS} -o ${CMAKE_BINARY_DIR}/test-cthread${EXESUF}
    BYPRODUCTS ${CMAKE_BINARY_DIR}/test-cthread${EXESUF}
    SOURCES ${TH_DIR}/test-cthread.c )
add_test(NAME pool-cthread COMMAND pool-cthread WORKING_DIRECTORY ${CMAKE_BINARY_DIR})


add_custom_target(
    pool-test_shutdown ALL
    COMMAND tcc ${TH_DIR}/pool-test_shutdown.c ${TH_CFLAGS} -o ${CMAKE_BINARY_DIR}/pool-test_shutdown${EXESUF}
    BYPRODUCTS ${CMAKE_BINARY_DIR}/pool-test_shutdown${EXESUF}
    SOURCES ${TH_DIR}/pool-test_shutdown.c )
add_test(NAME pool-test_shutdown COMMAND pool-test_shutdown WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(
    pool-test_heavy ALL
    COMMAND tcc ${TH_DIR}/pool-test_heavy.c ${TH_CFLAGS} -o ${CMAKE_BINARY_DIR}/pool-test_heavy${EXESUF}
    BYPRODUCTS ${CMAKE_BINARY_DIR}/pool-test_heavy${EXESUF}
    SOURCES ${TH_DIR}/pool-test_heavy.c )
add_test(NAME pool-test_heavy COMMAND pool-test_heavy WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(
    pool-test_wait ALL
    COMMAND tcc ${TH_DIR}/pool-test_wait.c ${TH_CFLAGS} -o ${CMAKE_BINARY_DIR}/pool-test_wait${EXESUF}
    BYPRODUCTS ${CMAKE_BINARY_DIR}/pool-test_wait${EXESUF}
    SOURCES ${TH_DIR}/pool-test_wait.c )
add_test(NAME pool-test_wait COMMAND pool-test_wait WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(
    test-atomicity ALL
    COMMAND tcc ${TH_DIR}/test-atomicity.c ${TH_CFLAGS} ${LIBTCC_LDFLAGS} -o ${CMAKE_BINARY_DIR}/test-atomicity${EXESUF}
    BYPRODUCTS ${CMAKE_BINARY_DIR}/test-atomicity${EXESUF}
    SOURCES ${TH_DIR}/test-atomicity.c )
add_test(NAME test-atomicity COMMAND test-atomicity WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_target(
    test-thread_local ALL
    COMMAND tcc ${TH_DIR}/test-thread_local.c ${TH_CFLAGS}  ${LIBTCC_LDFLAGS} -o ${CMAKE_BINARY_DIR}/test-thread_local${EXESUF}
    BYPRODUCTS ${CMAKE_BINARY_DIR}/test-thread_local${EXESUF}
    SOURCES ${TH_DIR}/test-thread_local.c )
add_test(NAME test-thread_local COMMAND test-thread_local WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
