cmake_minimum_required(VERSION 3.25)

project(VulkanApp LANGUAGES CXX)

# --------------------------------------------------
# Build type (Debug / Release)
# --------------------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
	endif()

	set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)


# --------------------------------------------------
# Output directories
# --------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


# --------------------------------------------------
# Compiler
# --------------------------------------------------
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json (nice for clangd)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------------------------------------
# Find system-installed packages
# --------------------------------------------------
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# --------------------------------------------------
# Recursively collect sources
# --------------------------------------------------
file(GLOB_RECURSE PROJECT_SOURCES
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.cxx
    ${CMAKE_SOURCE_DIR}/src/*.cc
)

file(GLOB_RECURSE PROJECT_HEADERS
    CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.hpp
    ${CMAKE_SOURCE_DIR}/src/*.h
)

# --------------------------------------------------
# Executable
# --------------------------------------------------
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

# --------------------------------------------------
# Debug / Release configuration
# --------------------------------------------------
target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		$<$<CONFIG:Debug>:_DEBUG>
		$<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(${PROJECT_NAME}
	PRIVATE
		$<$<CONFIG:Debug>:-O0 -g>
		$<$<CONFIG:Release>:-O3>
)


# --------------------------------------------------
# Include directories
# --------------------------------------------------
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# --------------------------------------------------
# Link libraries (system)
# --------------------------------------------------
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Vulkan::Vulkan
        glfw
        glm::glm
)

# --------------------------------------------------
# Warnings + nice defaults
# --------------------------------------------------
target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
)

# --------------------------------------------------
# Linux-specific stuff (usually auto-handled, but safe)
# --------------------------------------------------
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            dl
            pthread
    )
endif()
