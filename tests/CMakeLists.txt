include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

set(EXESUF)
set(TCC_CFLAGS -w -I. -I${SRC_DIR} -I${TCC_DIR}/tests -I${INC_DIR} -L${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} )
set(TCC_MATH_LDFLAGS)
set(LIBTCC_LDFLAGS)
if(WIN32)
  set(TCC_CFLAGS ${TCC_CFLAGS} -I${WIN_DIR}/include -I${WIN_DIR}/include/winapi )
  set(EXESUF .exe )
  set(LIBTCC_LDFLAGS -Wl,-rpath=${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  add_subdirectory(dll)
else()
  set(LIBTCC_LDFLAGS -pthread -Wl,-rpath=${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

add_subdirectory(cc)
add_subdirectory(thread)
if( ${TARGET_ARCH} STREQUAL i386 )
    set(TCC_CFLAGS ${TCC_CFLAGS} -fno-PIC -fno-PIE -Wl,-z,notext )
elseif( ${TARGET_ARCH} STREQUAL arm_eabi
    OR ${TARGET_ARCH} STREQUAL armv7
    OR ${TARGET_ARCH} STREQUAL arm64
    OR ${TARGET_ARCH} STREQUAL arm )
        set(TCC_CFLAGS ${TCC_CFLAGS} -marm )
endif()

set(TESTS2
 00_assignment
 01_comment
 02_printf
 03_struct
 04_for
 05_array
 06_case
 07_function
 08_while
 09_do_while
 10_pointer
 11_precedence
 12_hashdefine
 13_integer_literals
 14_if
 15_recursion
 16_nesting
 17_enum
 18_include
 19_pointer_arithmetic
 20_pointer_comparison
 21_char_array
 22_floating_point
 23_type_coercion
 24_math_library
 25_quicksort
 26_character_constants
 27_sizeof
 28_strings
 29_array_address
 31_args
 32_led
 33_ternary_op
 35_sizeof
 36_array_initialisers
 37_sprintf
 38_multiple_array_index
 39_typedef
 40_stdio
 41_hashif
 42_function_pointer
 43_void_param
 44_scoped_declarations
 45_empty_for
 47_switch_return
 48_nested_break
 49_bracket_evaluation
 50_logical_second_arg
 51_static
 52_unnamed_enum
 54_goto
 55_lshift_type
 60_errors_and_warnings
 61_integers
 64_macro_nesting
 67_macro_concat
 70_floating_point_literals
 71_macro_empty_arg
 72_long_long_constant
 73_arm64
 75_array_in_struct_init
 76_dollars_in_identifiers
 77_push_pop_macro
 78_vla_label
 79_vla_continue
 80_flexarray
 81_types
 82_attribs_position
 83_utf8_in_identifiers
 84_hex-float
 85_asm-outside-function
 86_memory-model
 87_dead_code
 88_codeopt
 89_nocode_wanted
 90_struct-init
 91_ptr_longlong_arith32
 92_enum_bitfield
 93_integer_promotion
 94_generic
 95_bitfields
 95_bitfields_ms
 96_nodata_wanted
 97_utf8_string_literal
 98_al_ax_extend
 99_fastcall
 100_c99array-decls
 101_cleanup
 102_alignas
 103_implicit_memmove
 104+_inline
 104_inline
 105_local_extern
 106_versym
 107_stack_safe
 108_constructor
 109_float_struct_calling
 110_average
 111_conversion
 112_backtrace
 113_btdll
 114_bound_signal
 115_bound_setjmp
 116_bound_setjmp2
 117_builtins
 118_switch
 119_random_stuff
 120+_alias
 120_alias
 121_struct_return
 122_vla_reuse
 123_vla_bug
 124_atomic_counter
 125_atomic_misc
 126_bound_global
 127_asm_goto
 128_run_atexit
 129_scopes
 130_large_argument
 131_return_struct_in_reg
 132_bound_test
)

list(REMOVE_ITEM TESTS2 22_floating_point)
list(REMOVE_ITEM TESTS2 24_math_library)
list(REMOVE_ITEM TESTS2 60_errors_and_warnings)
list(REMOVE_ITEM TESTS2 96_nodata_wanted)
list(REMOVE_ITEM TESTS2 99_fastcall)
list(REMOVE_ITEM TESTS2 104+_inline)
list(REMOVE_ITEM TESTS2 113_btdll)
list(REMOVE_ITEM TESTS2 120+_alias)

if(WIN32)
  list(REMOVE_ITEM TESTS2 106_versym)
  list(REMOVE_ITEM TESTS2 114_bound_signal)
  list(REMOVE_ITEM TESTS2 116_bound_setjmp2)
  list(REMOVE_ITEM TESTS2 117_builtins)
elseif( NOT APPLE AND ${TARGET_ARCH} STREQUAL arm64
  OR ${TARGET_ARCH} STREQUAL riscv64
  OR ${TARGET_ARCH} STREQUAL armv7
  OR ${TARGET_ARCH} STREQUAL arm_eabi
  OR ${TARGET_ARCH} STREQUAL arm )
    list(REMOVE_ITEM TESTS2 85_asm-outside-function )
    list(REMOVE_ITEM TESTS2 98_al_ax_extend )
    list(REMOVE_ITEM TESTS2 127_asm_goto )
elseif(APPLE)
    list(REMOVE_ITEM TESTS2 114_bound_signal )
    list(REMOVE_ITEM TESTS2 115_bound_setjmp )
    list(REMOVE_ITEM TESTS2 116_bound_setjmp2 )
    list(REMOVE_ITEM TESTS2 117_builtins )
endif()

set(PP
 01
 02
 03
 04
 05
 06
 07
 08
 09
 10
 11
 14
 15
 16
 17
 18
 19
 20
 21
 22
 pp-counter
)

set(TEST_CFLAGS ${TCC_CFLAGS} ${NATIVE_DEFINES} ${TCC_MATH_LDFLAGS} ${LIBTCC_LDFLAGS})
foreach(testpp ${PP})
  add_test(NAME ${testpp} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pp
           COMMAND tcc ${TEST_CFLAGS} -run -E ${testpp}.c  arg1 arg2 | ${DIFF} - ${testpp}.expect)
endforeach()

foreach(testfile ${TESTS2})
  add_test(NAME ${testfile} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests2
           COMMAND tcc ${TEST_CFLAGS} -dt -bt -b -run ${testfile}.c - arg1 arg2 arg3 arg4 | ${DIFF} - ${testfile}.expect)
endforeach()

set( TEST_DIR ${TCC_DIR}/tests )
add_custom_target(
    tcctest ALL
    COMMAND tcc ${TEST_DIR}/tcctest.c ${TEST_CFLAGS} -o ${CMAKE_BINARY_DIR}/tcctest${EXESUF}
    BYPRODUCTS ${CMAKE_BINARY_DIR}/tcctest${EXESUF}
    DEPENDS libarchive
    SOURCES ${TEST_DIR}/tcctest.c )

add_test(NAME tcctest COMMAND tcctest WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

if(${TARGET_ARCH} STREQUAL armv7
    OR ${TARGET_ARCH} STREQUAL arm64
    OR ${TARGET_ARCH} STREQUAL riscv64)
    add_executable(libtcc_test ${TEST_DIR}/libtcc_test.c)
    target_link_libraries(libtcc_test PUBLIC m dl libtcc Threads::Threads)
    add_dependencies(libtcc_test libarchive)
else()
    add_custom_target(
        libtcc_test ALL
        COMMAND tcc ${TEST_DIR}/libtcc_test.c ${TEST_CFLAGS} -o ${CMAKE_BINARY_DIR}/libtcc_test${EXESUF} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/libtcc${LIBSUF}
        BYPRODUCTS ${CMAKE_BINARY_DIR}/libtcc_test${EXESUF}
        SOURCES ${TEST_DIR}/libtcc_test.c
        DEPENDS libarchive)
endif()

add_test(NAME libtcc_test COMMAND libtcc_test -B../built -L../built -I../include -I../src WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
