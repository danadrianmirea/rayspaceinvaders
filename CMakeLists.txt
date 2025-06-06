cmake_minimum_required(VERSION 3.10)
project(SpaceInvaders VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set raylib path
set(RAYLIB_PATH "C:/raylib/raylib" CACHE PATH "Path to raylib source directory")

# Configure static linking
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

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
    src/explosion.cpp
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
    src/explosion.h
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
    # Hide console window in Release builds
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
    target_link_options(${PROJECT_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
    # Hide console window in Release builds for MinGW
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS_RELEASE "-mwindows"
    )
    message(STATUS "Building statically linked executable")
endif()

# Copy resources to build directory
file(COPY Graphics DESTINATION ${CMAKE_BINARY_DIR})
file(COPY Sounds DESTINATION ${CMAKE_BINARY_DIR})
file(COPY Font DESTINATION ${CMAKE_BINARY_DIR})

# Create zip file after build
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/zip_temp/${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/zip_temp/${PROJECT_NAME}/
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/Graphics ${CMAKE_CURRENT_BINARY_DIR}/zip_temp/${PROJECT_NAME}/Graphics
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/Sounds ${CMAKE_CURRENT_BINARY_DIR}/zip_temp/${PROJECT_NAME}/Sounds
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/Font ${CMAKE_CURRENT_BINARY_DIR}/zip_temp/${PROJECT_NAME}/Font
        COMMAND powershell -Command "Compress-Archive -Path '${CMAKE_CURRENT_BINARY_DIR}/zip_temp/*' -DestinationPath '${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.zip' -Force"
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/zip_temp
        COMMENT "Creating ${PROJECT_NAME}.zip"
    )
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/zip_temp/${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/zip_temp/${PROJECT_NAME}/
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/Graphics ${CMAKE_CURRENT_BINARY_DIR}/zip_temp/${PROJECT_NAME}/Graphics
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/Sounds ${CMAKE_CURRENT_BINARY_DIR}/zip_temp/${PROJECT_NAME}/Sounds
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/Font ${CMAKE_CURRENT_BINARY_DIR}/zip_temp/${PROJECT_NAME}/Font
        COMMAND zip -r ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.zip ${CMAKE_CURRENT_BINARY_DIR}/zip_temp/*
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/zip_temp
        COMMENT "Creating ${PROJECT_NAME}.zip"
    )
endif()