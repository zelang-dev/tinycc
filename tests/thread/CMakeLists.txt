include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${INC_DIR})

set( TH_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set(TH_CFLAGS -g -B ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} -I${INC_DIR})
if(WIN32)
  set(TH_CFLAGS ${TH_CFLAGS} -I${WIN_DIR}/include -I${WIN_DIR}/include/winapi)
  set(EXESUF .exe )
  set(LIBTCC_LDFLAGS -Wl,-rpath=${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else()
  set(TCC_MATH_LDFLAGS )
  set(LIBTCC_LDFLAGS -pthread -Wl,-rpath=${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

#if(NOT ${TARGET_ARCH} STREQUAL armv7
#AND NOT ${TARGET_ARCH} STREQUAL arm64
#    AND NOT ${TARGET_ARCH} STREQUAL riscv64)
    add_custom_target(
        test-cthread ALL
        COMMAND tcc ${TH_DIR}/test-cthread.c ${TH_CFLAGS} ${LIBTCC_LDFLAGS} -o ${CMAKE_BINARY_DIR}/test-cthread${EXESUF}
        BYPRODUCTS ${CMAKE_BINARY_DIR}/test-cthread${EXESUF}
        SOURCES ${TH_DIR}/test-cthread.c )
    add_test(NAME test-cthread COMMAND test-cthread WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    add_custom_target(
        test_pool-shutdown ALL
        COMMAND tcc ${TH_DIR}/test_pool-shutdown.c ${TH_CFLAGS} ${LIBTCC_LDFLAGS} -o ${CMAKE_BINARY_DIR}/test_pool-shutdown${EXESUF}
        BYPRODUCTS ${CMAKE_BINARY_DIR}/test_pool-shutdown${EXESUF}
        SOURCES ${TH_DIR}/test_pool-shutdown.c )
    add_test(NAME test_pool-shutdown COMMAND test_pool-shutdown WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    add_custom_target(
        test_pool-wait ALL
        COMMAND tcc ${TH_DIR}/test_pool-wait.c ${TH_CFLAGS} ${LIBTCC_LDFLAGS} -o ${CMAKE_BINARY_DIR}/test_pool-wait${EXESUF}
        BYPRODUCTS ${CMAKE_BINARY_DIR}/test_pool-wait${EXESUF}
        SOURCES ${TH_DIR}/test_pool-wait.c )
        add_test(NAME test_pool-wait COMMAND test_pool-wait WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

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
#endif()
