#include <iostream>
#include <string>
#include "lua.h"
#include "lualib.h"
#include "../Core/InstanceManager.hpp" // Assumes registry is defined here

// External reference to your global engine registry
extern InstanceRegistry g_Registry; 

// Helper struct for Luau Userdata wrapper
struct LuaInstance {
    uint32_t id;
};

// --- PROPERTY GETTERS (__index) ---
// Handles: print(part.Position) or local name = part.Name
int instance_index(lua_State* L) {
    // 1. Get the custom userdata from stack slot 1
    LuaInstance* instance = (LuaInstance*)luaL_checkudata(L, 1, "RogoModel_Instance");
    // 2. Get the property name string from stack slot 2
    std::string key = luaL_checkstring(L, 2);

    if (key == "Position") {
        // Fetch from flat array using ID
        Vector3 pos = g_Registry.GetPosition(instance->id);
        
        // Push 3 numbers onto stack for Luau to read
        lua_pushnumber(L, pos.x);
        lua_pushnumber(L, pos.y);
        lua_pushnumber(L, pos.z);
        return 3; // Returning 3 values (x, y, z)
    } 
    else if (key == "ID") {
        lua_pushinteger(L, instance->id);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// --- PROPERTY SETTERS (__newindex) ---
// Handles: part.Position = Vector3(10, 5, 2)
int instance_newindex(lua_State* L) {
    LuaInstance* instance = (LuaInstance*)luaL_checkudata(L, 1, "RogoModel_Instance");
    std::string key = luaL_checkstring(L, 2);

    if (key == "Position") {
        // Pull the new coordinates off the Luau stack
        float x = (float)luaL_checknumber(L, 3);
        float y = (float)luaL_checknumber(L, 4);
        float z = (float)luaL_checknumber(L, 5);

        // Directly write into the flat C++ memory array
        g_Registry.SetPosition(instance->id, {x, y, z});
    }

    return 0; // Setters do not return values
}

// --- INSTANCE FACTORY (Instance.new) ---
// Handles: local part = Instance.new("Part")
int instance_new(lua_State* L) {
    std::string className = luaL_checkstring(L, 1);

    if (className == "Part") {
        // 1. Allocate a lightweight part inside the engine arrays
        uint32_t newId = g_Registry.CreatePart({0, 0, 0}, {1, 1, 1}, {255, 255, 255});

        // 2. Create the lightweight Luau userdata memory
        LuaInstance* userdata = (LuaInstance*)lua_newuserdata(L, sizeof(LuaInstance));
        userdata->id = newId;

        // 3. Attach the metatable so Luau knows how to interface with it
        luaL_getmetfield(L, LUA_REGISTRYINDEX, "RogoModel_Instance");
        lua_setmetatable(L, -2);

        return 1; // Return the wrapped object back to Luau
    }

    lua_pushnil(L);
    return 1;
}

// --- INIT ---
// Call this function inside Main.cpp to bind everything to your Luau VM state
void RegisterEngineAPIs(lua_State* L) {
    // 1. Create the Instance metatable type globally in the VM
    luaL_newmetatable(L, "RogoModel_Instance");
    
    lua_pushcfunction(L, instance_index);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, instance_newindex);
    lua_setfield(L, -2, "__newindex");
    
    lua_pop(L, 1); // Clean metatable off stack

    // 2. Build the global "Instance" table library for scripts
    lua_newtable(L);
    
    lua_pushcfunction(L, instance_new);
    lua_setfield(L, -2, "new");
    
    lua_setglobal(L, "Instance");
}
