cmake_minimum_required(VERSION 3.22)
project(bezopastnost)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS program_options REQUIRED)
add_executable(lab2 main.cpp)
target_link_libraries(lab2 ${Boost_LIBRARIES})


find_library(LIBZ NAMES libzip libzip-dev libzip.so
        HINTS "/usr/lib/x86_64-linux-gnu" "/usr/lib/x86_64-linux-gnu/libzip.so")
add_executable(dummy dummy.cpp)
target_link_libraries(dummy ${LIBZ})


add_executable(hasher hasher.cpp)
find_library(LIBHASH NAMES libcrypto libcrypto-dev libcrypto.so
        HINTS "/usr/lib/x86_64-linux-gnu" "/usr/lib/x86_64-linux-gnu/libcrypto.so")
target_link_libraries(hasher ${LIBHASH} ${OpenSSL_LIBRARIES})


add_executable(splitter splitter.cpp)

