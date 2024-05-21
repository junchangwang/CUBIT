cmake_minimum_required(VERSION 3.2.0)

option(test "Build all tests." OFF) # Makes boolean 'test' available.

set(PROJECT_NAME nicolas)
set(LIB_NAME bitmap)

project(${PROJECT_NAME})

include_directories(${CMAKE_SOURCE_DIR}/src)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux
    set(CMAKE_CXX_FLAGS "-g -O3 -std=gnu++17 -fPIC")
    set(FASTBIT_LIBRARY "/usr/local/lib/libfastbit.so")
    include_directories("/usr/include/x86_64-linux-gnu")
    include_directories("/usr/local/include")
    set(BOOST_LIBRARYDIR /usr/lib/x86_64-linux-gnu)
    set(BOOST_INCLUDEDIR /usr/include)
    set(Boost_NO_SYSTEM_PATHS on)
    find_package(Boost COMPONENTS filesystem program_options system REQUIRED)

    ADD_DEFINITIONS( "-DLINUX" )
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

find_library(URCU_LIBRARIES 
		NAMES urcu
		HINTS "/usr/lib/x86_64-linux-gnu;/usr/local/lib")
#MESSAGE( FATAL_ERROR "${Boost_LIBRARIES}     dic: ${Boost_LIBRARY_DIRS}  ${Boost_INCLUDE_DIRS} ${URCU_LIBRARIES}")

# protobuf
#include(FindProtobuf)
#find_package(Protobuf REQUIRED)
#include_directories(${PROTOBUF_INCLUDE_DIR})
#include_directories(${PROJECT_SOURCE_DIR}/proto)
#PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders src/nicolas/proto/queries.proto)

# fastbit
set(FASTBIT_FILES src/fastbit/bitvector.cpp src/fastbit/bitvector.h
                  src/fastbit/fileManager.cpp src/fastbit/fileManager.h
                  src/fastbit/resource.cpp src/fastbit/resource.h
                  src/fastbit/util.cpp src/fastbit/util.h
                  src/fastbit/array_t.cpp src/fastbit/array_t.h)

# bitmap
set(NAIVE_SOURCE_FILES src/naive/table.h src/naive/table.cpp)
set(UCB_SOURCE_FILES src/ucb/table.h src/ucb/table.cpp)
set(UB_SOURCE_FILES src/ub/table.h src/ub/table.cpp)
set(NBUB_SOURCE_FILES src/cubit/table.h src/cubit/table.cpp src/cubit/table_lk.h src/cubit/table_lk.cpp src/cubit/table_lf.h src/cubit/table_lf.cpp src/cubit/test.cpp)
set(BASE_FILES src/nicolas/base_table.h src/nicolas/base_table.cpp src/nicolas/tablex.cpp src/nicolas/tablex.h src/nicolas/util.h src/nicolas/util.cpp src/nicolas/segBtv.h src/nicolas/segBtv.cpp src/nicolas/perf.h src/nicolas/perf.cpp)
add_library(bitmap STATIC ${BASE_FILES} ${FASTBIT_FILES} ${NAIVE_SOURCE_FILES} ${UCB_SOURCE_FILES} ${UB_SOURCE_FILES} ${NBUB_SOURCE_FILES})
target_link_libraries(bitmap pthread atomic ${Boost_LIBRARIES} ${URCU_LIBRARIES})
	  
# nicolas
set(NICOLAS_SOURCE_FILES src/nicolas/nicolas.cpp)
add_executable(nicolas ${NICOLAS_SOURCE_FILES})
target_link_libraries(nicolas bitmap pthread atomic ${Boost_LIBRARIES} ${URCU_LIBRARIES})


# bench
# add_executable(bench src/nicolas/bench.cpp src/nicolas/util.h src/nicolas/util.cpp ${ProtoSources} ${ProtoHeaders})
# target_link_libraries(bench ${Boost_LIBRARIES} ${FASTBIT_LIBRARY} ${PROTOBUF_LIBRARY})

# ucb
# add_executable(ucb src/ucb/ucb.cpp src/ucb/table.h src/ucb/table.cpp ${FASTBIT_FILES} src/nicolas/util.h src/nicolas/util.cpp)
# target_link_libraries(ucb ${Boost_LIBRARIES})

# naive
# add_executable(naive src/naive/naive.cpp src/naive/table.h src/naive/table.cpp ${FASTBIT_FILES} src/nicolas/util.h src/nicolas/util.cpp)
# target_link_libraries(naive ${Boost_LIBRARIES})

# part
# add_executable(part src/part/part.cpp src/part/table.h src/part/table.cpp ${FASTBIT_FILES} src/nicolas/util.h src/nicolas/util.cpp)
# target_link_libraries(part ${Boost_LIBRARIES})

# disk
# add_executable(disk src/disk/disk.cpp src/disk/table.h src/disk/table.cpp ${FASTBIT_FILES} src/nicolas/util.h src/nicolas/util.cpp)
# target_link_libraries(disk ${Boost_LIBRARIES})


################################
# Testing
################################
#if (test)
#  # This adds another subdirectory, which has 'project(gtest)'.
#  add_subdirectory(src/gtest)
#
#  enable_testing()
#
#  # Include the gtest library. gtest_SOURCE_DIR is available due to
#  # 'project(gtest)' above.
#  # include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
#
#  ##############
#  # Unit Tests
#  ##############
#  add_executable(runUnitTests src/nicolas/test/test_common.cpp)
#
#  # Standard linking to gtest stuff.
#  target_link_libraries(runUnitTests gtest gtest_main)
#
#  # Extra linking for the project.
#  target_link_libraries(runUnitTests ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
#
#  # You can also omit NAME and COMMAND. The second argument could be some other
#  # test executable.
#  add_test(common-test runUnitTests)
#endif()
