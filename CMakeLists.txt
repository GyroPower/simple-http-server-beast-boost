cmake_minimum_required(VERSION 3.26.3)

if (WIN32)
    cmake_policy(SET CMP0167 NEW)
endif()

project(advanced_server_beast)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# Find Boost libraries
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})


# if(GCC)
#      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++14")
#      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
# endif()

# file (GLOB APP_FILES
#     main2.cpp
#     beast.hpp
#     http_session.cpp 
#     http_session.hpp
#     listener.cpp 
#     listener.hpp
#     net.hpp
#     shared_state.cpp 
#     shared_state.hpp 
#     websocket_session.cpp 
#     websocket_session.hpp 


# )


# Add executable
add_executable(simpleHttpServer-boost-beast main.cpp)

# Link libraries
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
    target_link_libraries(simpleHttpServer-boost-beast ws2_32 mswsock ${Boost_LIBRARIES} )
else()
    target_link_libraries(simpleHttpServer-boost-beast ${Boost_LIBRARIES})
endif()


set(SOURCE_EXTERNAL_FILES
    index.html
)

set(DEST_DIR ${CMAKE_BINARY_DIR})

foreach(FILE ${SOURCE_EXTERNAL_FILES})
    add_custom_command(
        TARGET simpleHttpServer-boost-beast POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FILE} ${DEST_DIR}
        COMMENT "Copying ${FILE} to ${DEST_DIR}"
    )
endforeach()
