cmake_minimum_required(VERSION 3.12)
project(p1_project C)

set(CMAKE_C_STANDARD 11)

include_directories(includes)

add_compile_options(-Wall -Wextra -pedantic)

macro(build_file_object filename)
    add_custom_command(OUTPUT ${filename}.o
            COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/resources && ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/${filename}.o ${filename})
    SET_SOURCE_FILES_PROPERTIES(
            ${filename}.o
            PROPERTIES
            EXTERNAL_OBJECT true
            GENERATED true
    )
endmacro()

add_library(core_lib includes/generate.c includes/input.c includes/outputTable.c includes/types.c includes/weight_day.c includes/error.h includes/error.c includes/criteria.c includes/util.c)

build_file_object(header)
build_file_object(footer)

#add_library(xml_res STATIC header.o footer.o)
add_library(xml_lib includes/xmlWriter.c header.o footer.o)
#SET_TARGET_PROPERTIES(
#        xml_res
#        PROPERTIES
#        LINKER_LANGUAGE C
#)

add_executable(p1_project main.c)
add_executable(type_tests tests/type-tests.c)

enable_testing()
add_test(p1_project type_tests)

target_link_libraries(core_lib m)
target_link_libraries(xml_lib core_lib)
target_link_libraries(type_tests core_lib)
target_link_libraries(p1_project core_lib xml_lib)


include(.local/local.cmake OPTIONAL)
