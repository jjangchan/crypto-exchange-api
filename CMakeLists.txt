cmake_minimum_required(VERSION 3.22)
project(crypto_exchange_api)

set(CMAKE_CXX_STANDARD 14)


###################################################################
# OS Config
if(APPLE)
    message(STATUS ">>> OS: APPLE")
elseif(LINUX OR UNIX)
    message(STATUS ">>> OS: LINUX OR UNIX")
    set(CMAKE_BUILD_TYPE Release)
elseif(WIN32)
    message(STATUS ">>> OS: WINDOWS")
    set(CMAKE_CXX_FLAGS "-fexec-charset=CP949")
endif()
###################################################################

SET(SRC
        main.cpp
        src/ExchangeManageMent.h
        src/Errors.h
        src/BinanceAPI.h
        src/ExchangeType.h
        src/BithumbAPI.h
        src/UpbitAPI.h src/CoinBaseAPI.h)

link_directories(/usr/local/lib /usr/local/opt/openssl@3/lib)
include_directories(../include )

add_executable(crypto_exchange_api ${SRC})


######################################################################
# THREAD
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
# add_executable
target_link_libraries(${PROJECT_NAME} Threads::Threads)
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
        1.82.0
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


include_directories(/usr/local/include)

######################################################################
# OPENSSL
include_directories(/usr/local/opt/openssl@3/include)
target_link_libraries(${PROJECT_NAME} ssl crypto)
######################################################################

add_subdirectory(unittest)

