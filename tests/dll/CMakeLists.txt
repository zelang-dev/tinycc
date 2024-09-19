include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

if(WIN32)
    set( DL_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
    set( DL_SRCFILES
     testdll
     testdll2
     testdll3 )

    set(CFLAGS -I${CMAKE_SOURCE_DIR}/include -I${CMAKE_SOURCE_DIR}/win32/include -I${CMAKE_SOURCE_DIR}/win32/include/winapi -B${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
    foreach(DL_SRCFILE ${DL_SRCFILES})
        add_custom_command(
            OUTPUT ${CMAKE_BINARY_DIR}/${DL_SRCFILE}.dll
            COMMAND tcc -c ${DL_DIR}/${DL_SRCFILE}.c ${CFLAGS} -w -shared -o ${CMAKE_BINARY_DIR}/${DL_SRCFILE}.dll )
        list(APPEND DL_DLLFILES ${CMAKE_BINARY_DIR}/${DL_SRCFILE}.dll)
    endforeach()

    add_custom_target(
        test_win_dl ALL
        COMMAND tcc ${DL_DIR}/test.c ${CFLAGS} -B ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} -o ${CMAKE_BINARY_DIR}/test_win_dl.exe
        DEPENDS ${DL_DLLFILES}
        BYPRODUCTS ${CMAKE_BINARY_DIR}/test_win_dl.exe
        SOURCES ${DL_DIR}/test.c )

    add_test(NAME test_win_dl COMMAND test_win_dl WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
