cmake_minimum_required(VERSION 3.22)
project(unittest)


set(CMAKE_CXX_STANDARD 14)
SET(SRC
        BinanceTest.cpp
        BithumbTest.cpp
        CoinBaseTest.cpp
        UpbitTest.cpp
        BInanceOrderTest.cpp
        )

link_directories(/usr/local/lib /usr/local/opt/openssl@3/lib)
include_directories(../include)

if(UNIX AND NOT APPLE)
    link_libraries(uuid)
endif()

add_executable(${PROJECT_NAME} ${SRC})


######################################################################
# THREAD
if(UNIX)
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads REQUIRED)
    # add_executable
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
endif()
######################################################################


######################################################################
# CURL
find_package(CURL REQUIRED)
if(CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIR})
    # CURL Libraries
    target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})
    message(STATUS "CURL Version: ${CURL_VERSION_STRING}")
else()
    message(FATAL_ERROR "Could not find Curl")
endif()
######################################################################

######################################################################
# Boost
set(Boost_USE_STATIC_LIBS        OFF)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
set(Boost_NO_BOOST_CMAKE         ON)
find_package(Boost
        1.79.0
        COMPONENTS
        program_options
        thread
        log_setup
        log
        system
        date_time
        filesystem
        regex
        chrono
        atomic
        REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set(Boost_LIB
            ${Boost_LIBRARIES}
            #        ${Boost_SYSTEM_LIBRARY}
            #        ${Boost_LOG_SETUP_LIBRARY}
            #        ${Boost_LOG_LIBRARY}
            )
    add_definitions(-DBOOST_LOG_DYN_LINK=1)
    message(STATUS "Boost Version: ${Boost_VERSION}")
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_LOG_LIBARY: ${Boost_LOG_LIBRARY}")
else()
    message(FATAL_ERROR "Could not find Boost")
endif()
# BOOST Libraries
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
######################################################################

######################################################################
# OPENSSL
include_directories(/usr/local/opt/openssl@3/include)
target_link_libraries(${PROJECT_NAME} ssl crypto)
######################################################################

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${INCDIR})

if(APPLE)
    include_directories(/usr/local/include)
elseif(LINUX OR UNIX)
    include_directories(/usr/local/include)
elseif(WIN32)
    #    include_directories(/usr/local/include)
endif()

target_link_libraries(${PROJECT_NAME} gtest gtest_main)
