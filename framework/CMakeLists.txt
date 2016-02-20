cmake_minimum_required(VERSION 2.8)
project(sedna-testing)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include_directories(${CMAKE_SOURCE_DIR})

include_directories("/home/alexander/software/sedna/driver/c")
link_directories("/home/alexander/software/sedna/driver/c")

find_package(Boost COMPONENTS system filesystem regex REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(PugiXML REQUIRED)

pkg_check_modules(LIBXMLPP REQUIRED libxml++-2.6)
pkg_check_modules(GLIB REQUIRED glib-2.0)

include_directories(${LIBXMLPP_INCLUDE_DIRS})
link_directories(${LIBXMLPP_LIBRARY_DIRS})

include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})

include_directories(/usr/include/glib-2.0)

add_subdirectory(sedna)
add_subdirectory(XQTS)
add_subdirectory(Tester)
add_subdirectory(diff)

AUX_SOURCE_DIRECTORY(XQTS XQTS_SOURCES) 
AUX_SOURCE_DIRECTORY(sedna SEDNA_SOURCES) 

option(SETEST_DEBUG "Debuggiong setest" OFF)

add_executable(sedna-testing ${XQTS_SOURCES} ${SEDNA_SOURCES} diff/xmldiff.cpp environment/environment.cpp Tester/Tester.cpp main.cpp)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++0x -g3 -ggdb")
if(SETEST_DEBUG)
    message(STATUS "bleep-bloop")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSETEST_DEBUG")
endif(SETEST_DEBUG)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=c++0x -g3 -ggdb")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSETEST_DEBUG")

target_link_libraries(sedna-testing
  ${PugiXML_LIBRARY}
  ${LIBXMLPP_LIBRARIES}
  ${BOOST_FILESYSTEM_LIBRARY}
  ${BOOST_SYSTEM_LIBRARY}
  ${BOOST_REGEX_LIBRARY}
  ${Boost_LIBRARIES} 
  rt
  sedna
)
