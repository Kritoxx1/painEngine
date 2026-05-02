cmake_minimum_required(VERSION 3.20)
project(painEngine)
 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



# ── Source files ──────────────────────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/pain_app.cpp
    src/pain_window.cpp
    src/pain_pipeline.cpp
    src/pain_device.cpp
    src/pain_swapchain.cpp
    src/pain_model.cpp
    src/pain_input.cpp
    src/pain_camera.cpp
    src/pain_uniform.cpp
)
 
add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_definitions(
        ${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:_PAIN_DEBUG>
)

target_compile_options(
        ${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-g>
)

# Compile Shaders
add_custom_target(compile_shaders ALL
    COMMAND ${CMAKE_SOURCE_DIR}/compile.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running compile.sh"
)

# ── Include directories ───────────────────────────────────────────────────────
target_include_directories(${PROJECT_NAME} PRIVATE
    include
    Libs/include
    ext
)

# Subdirectories
add_subdirectory(glfw)
include_directories(glfw/include)

add_subdirectory(Vulkan-Headers)
include_directories(Vulkan-Headers/include)

add_subdirectory(glm)
include_directories(glm/glm)

# ── Link libraries ────────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw 
    Vulkan::Headers
)

# TODO: Implement mac support.
if(UNIX)
    find_package(Vulkan REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
elseif(WIN32)
    find_package(Vulkan REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
endif()
