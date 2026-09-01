# Engine CMakeLists
set(ENGINE_SOURCES
    "engine/core/Window.h"
    "engine/core/Window.cpp"
    "engine/core/Application.h"
    "engine/core/Application.cpp"
    "engine/core/Log.h"         
    "engine/core/Log.cpp"        
    "engine/core/Layer.h"       
    "engine/core/LayerStack.h"   
    "engine/core/LayerStack.cpp" 
)

# Compile the core engine as a static library
add_library(EngineCore STATIC ${ENGINE_SOURCES} "engine/core/Log.h" "engine/core/Log.cpp" "engine/core/LayerStack.h" "engine/core/LayerStack.cpp")

# Expose include directories so files can find each other via clean paths
target_include_directories(EngineCore PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../external
)

# Link hardware dependencies directly into your engine library
target_link_libraries(EngineCore PUBLIC glfw glad)



# B. BUILD THE RUNNABLE GAME TARGET (SANDBOX)
set(SANDBOX_SOURCES
    "sandbox/main.cpp"
)

# Create the final desktop executable 
add_executable(SandboxApp ${SANDBOX_SOURCES} "engine/core/Log.h" "engine/core/Log.cpp" "engine/core/LayerStack.h" "engine/core/LayerStack.cpp")

# Link your sandbox executable directly against your custom engine
target_link_libraries(SandboxApp PRIVATE EngineCore)

# Allow the sandbox to find your assets folder easily
target_compile_definitions(SandboxApp PRIVATE 
    ENGINE_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../assets/"
)
