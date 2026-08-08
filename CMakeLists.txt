cmake_minimum_required(VERSION 3.20)
project(RogoModel VERSION 0.1.0 LANGUAGES CXX C)

# Force modern C++ standards for optimized memory alignment
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------------------------
# Dependencies Tracking (GLFW3 & OpenGL)
# -------------------------------------------------------------------
# Ensure you have your system packages installed or compiled locally
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# -------------------------------------------------------------------
# Engine Engine Source Definitions
# -------------------------------------------------------------------
# Explicitly listing sources keeps the project highly organized and maintainable
set(ENGINE_SOURCES
    src/Main.cpp
    
    src/Core/InstanceManager.hpp
    
    src/Scripting/LuauBindings.hpp
    src/Scripting/LuauBindings.cpp
    
    src/Graphics/Renderer.hpp
    src/Graphics/Renderer.cpp
)

# -------------------------------------------------------------------
# Third-Party Inclusions (GLAD & Luau Embedded Library)
# -------------------------------------------------------------------
# If you are embedding GLAD and Luau source files inside ThirdParty/
include_directories(
    include
    ThirdParty/luau/VM/src
    ThirdParty/luau/Common/include
)

# -------------------------------------------------------------------
# Build Binary Executable Target
# -------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${ENGINE_SOURCES})

# Target Linkage
# Directs the compiler to stitch external libraries to your core executable
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    glfw
    OpenGL::GL
    # luau_vm  # Uncomment this once your Luau submodule is compiled via add_subdirectory
)

# Setup runtime working directory so the engine can easily load standalone script assets
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
