CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

SET(CMAKE_BUILD_TYPE Debug)

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(PROJECT db_tool)
PROJECT(${PROJECT} LANGUAGES CXX)

FIND_PACKAGE(SQLite3 REQUIRED)
INCLUDE_DIRECTORIES(${SQLite3_INCLUDE_DIRS})

FIND_PACKAGE(Boost REQUIRED )
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

ADD_EXECUTABLE(${PROJECT} 
    src/data_types.hpp
    src/main__.cpp
    src/main.cpp
    src/IDbReader.hpp
    src/dbreader.cpp
    src/dbreader.h
    src/dbcompare.cpp
    src/dbcompare.h
    src/global_defines.hpp  
    src/trigger_funcs.hpp 
    src/trigger.cpp 
    src/trigger.h
    src/global_funcs.hpp
    src/test_funcs__.hpp
    src/test_funcs.hpp
    src/table.cpp 
    src/table.h
    src/table_funcs.hpp 
    src/commands.cpp
    src/commands.h
    src/commands_funcs.hpp
    src/dump.cpp
    src/dump.h
    src/dump_builder.cpp
    src/dump_builder.h
    src/table_builder.hpp
    src/trigger_builder.hpp
    src/log.cpp
    src/log.h
    src/log_builder.hpp
)

TARGET_LINK_LIBRARIES(${PROJECT} ${SQLite3_LIBRARIES})
TARGET_LINK_LIBRARIES(${PROJECT} ${Boost_LIBRARIES})