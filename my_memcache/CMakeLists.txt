set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

find_package (Threads REQUIRED)
find_package(Boost COMPONENTS system context regex REQUIRED)
FIND_LIBRARY(SQLITE3_LIBRARY NAMES sqlite3)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})
link_directories(/usr/lib)

include_directories(src hdr)

include(../PrecompiledHeader.cmake)
add_library(pch src/stdafx.cpp src/stdafx.h)
add_precompiled_header(pch src/stdafx.h FORCEINCLUDE)

file(GLOB SOURCES "src/*.cpp")

add_executable(
	my_memcache 
		${SOURCES}
)
include_directories(${SQLITE3_INCLUDE_DIRS})

target_link_libraries(my_memcache ${Boost_LIBRARIES} )
target_link_libraries(my_memcache ${SQLITE3_LIBRARIES})
target_link_libraries(my_memcache ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(my_memcache sqlite3)
