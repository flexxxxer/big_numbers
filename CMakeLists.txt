cmake_minimum_required (VERSION 3.14)
set(CMAKE_CXX_STANDARD 20)

project ("big_numbers" VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_VERSION 3.14)
set(CMAKE_CXX_STANDARD 20)

file(GLOB SOURCE_FILES
    #"*.h"
    "*.cpp"
	"examples/*.cpp"
	"tests/*.cpp"
)

add_executable(big_numbers ${SOURCE_FILES})

option(TESTING "Run tests" on)
option(PERFORMANCE_TESTING "Run performance tests" off)

if(TESTING)
	add_definitions (-DTESTING_ENABLED=1)
endif()

if(PERFORMANCE_TESTING)
	add_definitions (-DPERFORMANCE_TESTING_ENABLED=1)
endif()

# googletest
add_subdirectory(tests/libs/googletest)
include_directories("tests/libs/googletest/googletest/include")
target_link_libraries(big_numbers gtest gtest_main)

# big_numbers
add_subdirectory(src/big_numbers)
include_directories("src")

#tests
add_subdirectory("tests")

if(TESTING)
	if(MSVC)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
	endif()
endif()