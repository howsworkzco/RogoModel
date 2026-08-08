#include <iostream>
#include "Core/InstanceManager.hpp"
#include "Scripting/LuauBindings.hpp"
#include "Graphics/Renderer.hpp"

// Allocate the global, data-oriented memory database.
// This is declared as 'extern' in LuauBindings.cpp so the scripting layer can reach it.
InstanceRegistry g_Registry;

int main() {
    std::cout << "[RogoModel] Launching lightweight engine core..." << std::endl;

    // 1. Initialize the ultra-lightweight hardware rendering window
    RogoRenderer renderer;
    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;
    
    if (!renderer.Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, "RogoModel Alpha Client")) {
        std::cerr << "[RogoModel Error] Failed to initialize GLFW or OpenGL context!" << std::endl;
        return -1;
    }
    std::cout << "[RogoModel] Graphics pipeline loaded successfully via OpenGL." << std::endl;

    // 2. Initialize and configure the Luau Virtual Machine
    lua_State* L = luaL_newstate();
    if (!L) {
        std::cerr << "[RogoModel Error] Failed to initialize Luau VM environment!" << std::endl;
        return -1;
    }

    // Load standard safe math and string libraries into Luau
    luaL_openlibs(L); 

    // Register our custom engine syntax rules (like 'Instance.new') into the VM
    RegisterEngineAPIs(L);
    std::cout << "[RogoModel] Luau script environment successfully injected." << std::endl;

    // 3. Execute a simulation test script (Roblox/Polytoria syntax style)
    // This creates two distinct structural parts directly into our flat memory arrays.
    const char* testScript = 
        "print('[Luau] Executing RogoModel runtime test...');"
        "local block1 = Instance.new('Part');"
        "block1.Position = -1.5, 0.0, -4.0;"
        ""
        "local block2 = Instance.new('Part');"
        "block2.Position = 1.5, 0.0, -4.0;";

    std::cout << "[RogoModel] Compiling and running internal test script..." << std::endl;
    if (luaL_dostring(L, testScript) != 0) {
        std::cerr << "[Luau Compiler Error] " << lua_tostring(L, -1) << std::endl;
    }

    // 4. Infinite High-Performance Game Engine Loop
    std::cout << "[RogoModel] Entering main engine simulation loop." << std::endl;
    while (!renderer.ShouldClose()) {
        // Collect OS signals (keyboard inputs, window resize, close buttons)
        renderer.PollEvents();

        // Pass our clean flat-array structures straight to VRAM for high-speed instanced drawing
        renderer.RenderFrame(g_Registry);
    }

    // 5. Cleanup memory systems gracefully when shutting down the program
    std::cout << "[RogoModel] Shutting down execution contexts..." << std::endl;
    lua_close(L);
    
    return 0;
}
