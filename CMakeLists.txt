cmake_minimum_required(VERSION 3.30)

# The toolchain file setup should ideally be in a CMakePresets.json file,
# but we'll keep it here for simplicity
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

project(ZombieDontKillMe VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add dependencies
find_package(SFML REQUIRED COMPONENTS Graphics Window System Audio)
find_package(glm CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# No C++20 modules
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Game executable
add_executable(ZombieDontKillMe)

# Game sources and headers
file(GLOB_RECURSE ZombieDontKillMe_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
)

target_sources(ZombieDontKillMe PRIVATE ${ZombieDontKillMe_SOURCES})

# Game include dirs
target_include_directories(ZombieDontKillMe PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Preprocessor defines
target_compile_definitions(ZombieDontKillMe PRIVATE
    MY_TEST_DEFINE
)

# Automatically links to these libs
target_link_libraries(ZombieDontKillMe PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    glm::glm
    nlohmann_json::nlohmann_json
)

# Add directories with any random loose .dlls or .libs here
target_link_directories(ZombieDontKillMe PRIVATE

)

# Copy the assets folder next to the built executable so the game can find
# textures/sounds/fonts/data at runtime without an install step
add_custom_command(TARGET ZombieDontKillMe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:ZombieDontKillMe>/assets"
)

# Set configuration properties for ZombieDontKillMe
if(MSVC)
    target_compile_options(ZombieDontKillMe PRIVATE
        /W3
        /MP # multithreaded build
        /WX # warnings as errors
        /ZI # program database for edit and continue
    )
else()
    target_compile_options(ZombieDontKillMe PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# Set ZombieDontKillMe as the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ZombieDontKillMe)

# Filters
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${ZombieDontKillMe_SOURCES})

# Enable solution folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
