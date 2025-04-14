cmake_minimum_required(VERSION 3.10)
project(SpaceInvaders VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set raylib path
set(RAYLIB_PATH "C:/raylib/raylib" CACHE PATH "Path to raylib source directory")

# Set source files
set(SOURCES
    src/main.cpp
    src/game.cpp
    src/laser.cpp
    src/alien.cpp
    src/spaceship.cpp
    src/globals.cpp
    src/misteryship.cpp
    src/obstacle.cpp
    src/block.cpp
)

# Set header files
set(HEADERS
    src/laser.h
    src/alien.h
    src/game.h
    src/spaceship.h
    src/globals.h
    src/misteryship.h
    src/obstacle.h
    src/block.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Add raylib as a subdirectory
add_subdirectory(${RAYLIB_PATH} ${CMAKE_BINARY_DIR}/raylib)

# Link against raylib
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    src
    ${RAYLIB_PATH}/src
)

# Set compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
)

# Set compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# Copy resources to build directory
file(COPY Graphics DESTINATION ${CMAKE_BINARY_DIR})
file(COPY Sounds DESTINATION ${CMAKE_BINARY_DIR})
file(COPY Font DESTINATION ${CMAKE_BINARY_DIR})
file(COPY highscore.txt DESTINATION ${CMAKE_BINARY_DIR}) 