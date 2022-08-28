set(BOOST_VERSION "1.80.0")
set(BOOST_ROOT "C:/Program Files/boost/boost_1_80_0")
set(BOOST_INCLUDEDIR ${BOOST_ROOT})
#set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib64-msvc-14.2)

set(Boost_NO_WARN_NEW_VERSIONS 1)
set(BOOST_WHATEVER_NO_LIB ON)
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_DEBUG_LIBS ON)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
#find_package(Boost ${BOOST_VERSION} COMPONENTS context chrono date_time filesystem system unit_test_framework thread regex serialization wserialization iostreams program_options 
    # REQUIRED      # due to boost setup is optional in this repo
#)
set(Boost_INCLUDE_DIR "C:/Program Files/boost/boost_1_80_0")
find_package(Boost ${BOOST_VERSION} REQUIRED COMPONENTS thread system chrono)

add_library(boost INTERFACE)
target_include_directories(boost INTERFACE ${BOOST_INCLUDEDIR})
target_link_libraries(boost INTERFACE Boost::thread Boost::system Boost::chrono)