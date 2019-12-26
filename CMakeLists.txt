set(binbro_INC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(binbro_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/binbro/binbro.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/binbro/rc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/binbro.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/fileutils.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/md5.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/md5.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fileutils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rc.cpp)

add_library(binbro STATIC ${binbro_SRC})

target_include_directories(binbro
    PUBLIC
        $<BUILD_INTERFACE:${binbro_INC}>)

target_link_libraries(binbro)

#cli
set(binbro_cli_SRC cli/main.cpp)

add_executable(binbro_cli ${binbro_cli_SRC})

target_link_libraries(binbro_cli binbro)

#test
set(binbro_test_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/test_api.cpp)

add_executable(binbro_test
    ${binbro_test_SRC})

target_link_libraries(binbro_test
    binbro
    gtest)