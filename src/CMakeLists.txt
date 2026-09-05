# Engine CMakeLists
set(ENGINE_SOURCES
    # CORE
    "engine/core/Window.h"
    "engine/core/Window.cpp"
    "engine/core/Application.h"
    "engine/core/Application.cpp"
    "engine/core/Log.h"         
    "engine/core/Log.cpp"        
    "engine/core/Layer.h"       
    "engine/core/LayerStack.h"   
    "engine/core/LayerStack.cpp" 

    # Renderer
    "engine/renderer/Shader.h"
    "engine/renderer/Shader.cpp"
    "engine/renderer/VertexArray.h"
    "engine/renderer/VertexArray.cpp"
    "engine/renderer/Buffer.h"
    "engine/renderer/Buffer.cpp"
    "engine/renderer/Camera.h"
    "engine/renderer/Camera.cpp"
    "engine/renderer/Texture.h"
    "engine/renderer/Texture.cpp"
    "engine/renderer/Framebuffer.h"
    "engine/renderer/Framebuffer.cpp"

    # Resources 
    "engine/resources/ModelLoader.h"
    "engine/resources/ModelLoader.cpp"

    # DEAR IMGUI SOURCE FILES (Compiled directly into Engine)
    "../external/imgui/imgui.cpp"
    "../external/imgui/imgui_draw.cpp"
    "../external/imgui/imgui_tables.cpp"
    "../external/imgui/imgui_widgets.cpp"
    "../external/imgui/backends/imgui_impl_glfw.cpp"
    "../external/imgui/backends/imgui_impl_opengl3.cpp"
)

# Disable Assimp's unit tests and sample apps (removes a huge chunk of files)
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "")
set(ASSIMP_BUILD_SAMPLES OFF CACHE INTERNAL "")

# Disable all formats first, then explicitly turn on the one(s) you want
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE INTERNAL "")
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE INTERNAL "")  # Turn on .obj loader
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE INTERNAL "") # Turn on .gltf loader

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/assimp ${CMAKE_CURRENT_BINARY_DIR}/assimp_build)


# Compile the core engine as a static library (FIXED: Removed broken Level.cpp references)
add_library(EngineCore STATIC ${ENGINE_SOURCES})


# Expose include directories so files can find each other via clean paths
target_include_directories(EngineCore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../external 
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/assimp
    ${CMAKE_CURRENT_SOURCE_DIR}/../external/assimp/include
)

# Tell ImGui to use GLAD loader hooks for binding OpenGL functions
target_compile_definitions(EngineCore PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

# Link hardware dependencies directly into your engine library
target_link_libraries(EngineCore PUBLIC glfw glad assimp)


# B. BUILD THE RUNNABLE GAME TARGET (SANDBOX)
set(SANDBOX_SOURCES
    "sandbox/main.cpp"
    "sandbox/GameLayer.h"
    "sandbox/GameLayer.cpp" 
)

# Create the final desktop executable (FIXED: Removed broken Level.cpp references)
add_executable(SandboxApp ${SANDBOX_SOURCES})

# Link your sandbox executable directly against your custom engine
target_link_libraries(SandboxApp PRIVATE EngineCore)

# Allow the sandbox to find your assets folder easily
target_compile_definitions(SandboxApp PRIVATE 
    ENGINE_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/sandbox/assets/"
)
