#pragma once

#include "lua.h"
#include "lualib.h"

// Forward declaration of your registry class to avoid circular dependencies
class InstanceRegistry;

/**
 * Initializes and binds the RogoModel Instance library to the Luau VM State.
 * This registers custom Global types like `Instance` and sets up the metatable engine hooks.
 * 
 * @param L The active Luau VM state pointer.
 */
void RegisterEngineAPIs(lua_State* L);
