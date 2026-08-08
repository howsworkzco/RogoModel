#include <iostream>
#include "Core/InstanceManager.hpp"
#include "Core/Reflection.hpp"      // 1. Include the new reflection header
#include "Scripting/LuauBindings.hpp"
#include "Graphics/Renderer.hpp"

// Global instances so they can be accessed by the Luau bindings layer
InstanceRegistry g_Registry;
ReflectionSystem g_Reflection;       // 2. Allocate the global reflection engine

int main() {
    std::cout << "[RogoModel] Launching lightweight engine core..." << std::endl;

    // 3. CRUCIAL: Load Class Layout Configs FIRST before doing anything else
    if (!g_Reflection.LoadFromDisk("Reflection.json")) {
        std::cerr << "[RogoModel Error] Critical Failure: Could not load Reflection.json!" << std::endl;
        return -1; 
    }
    std::cout << "[RogoModel] Reflection database successfully initialized from disk." << std::endl;

    // 4. Initialize the ultra-lightweight hardware rendering window
    RogoRenderer renderer;
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    
    if (!renderer.Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, "RogoModel Alpha Client")) {
        std::cerr << "[RogoModel Error] Failed to initialize GLFW or OpenGL context!" << std::endl;
        return -1;
    }
    std::cout << "[RogoModel] Graphics pipeline loaded successfully via OpenGL." << std::endl;

    // 5. Initialize and configure the Luau Virtual Machine
    lua_State* L = luaL_newstate();
    if (!L) {
        std::cerr << "[RogoModel Error] Failed to initialize Luau VM environment!" << std::endl;
        return -1;
    }

    luaL_openlibs(L); 
    RegisterEngineAPIs(L); // The APIs will now use g_Reflection to validate data
    std::cout << "[RogoModel] Luau script environment successfully injected." << std::endl;

    // 6. Execute your simulation test script
    const char* testScript = 
        "print('[Luau] Executing dynamic reflection test...');"
        "local block1 = Instance.new('Part');"
        "block1.Position = -1.5, 0.0, -4.0;"
        "local script1 = Instance.new('Script');"
        "script1.Name = 'MainLogic';";

    std::cout << "[RogoModel] Compiling and running internal test script..." << std::endl;
    if (luaL_dostring(L, testScript) != 0) {
        std::cerr << "[Luau Compiler Error] " << lua_tostring(L, -1) << std::endl;
    }

    // 7. Infinite High-Performance Game Engine Loop
    std::cout << "[RogoModel] Entering main engine simulation loop." << std::endl;
    while (!renderer.ShouldClose()) {
        renderer.PollEvents();
        renderer.RenderFrame(g_Registry);
    }

    std::cout << "[RogoModel] Shutting down execution contexts..." << std::endl;
    lua_close(L);
    
    return 0;
}
