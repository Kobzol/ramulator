cmake_minimum_required(VERSION 3.10)
project(ramulator)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")

add_definitions(-DRAMULATOR)

set(SOURCES
        src/ALDRAM.cpp
        src/Cache.cpp
        src/Config.cpp
        src/Controller.cpp
        src/DDR3.cpp
        src/DDR4.cpp
        src/DSARP.cpp
        src/GDDR5.cpp
        src/Gem5Wrapper.cpp
        src/HBM.cpp
        src/LPDDR3.cpp
        src/LPDDR4.cpp
        src/Main.cpp
        src/MemoryFactory.cpp
        src/PCM.cpp
        src/Processor.cpp
        src/Refresh.cpp
        src/SALP.cpp
        src/StatType.cpp
        src/STTMRAM.cpp
        src/TLB.cpp
        src/TLDRAM.cpp
        src/WideIO.cpp
        src/WideIO2.cpp
)

add_executable(ramulator ${SOURCES})
