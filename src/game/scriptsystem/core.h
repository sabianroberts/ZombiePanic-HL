// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef SCRIPT_SYSTEM_CORE
#define SCRIPT_SYSTEM_CORE
#pragma once

#include <string>   // std::string

// Our script callback
#include "script_callback.h"

// Input/Output System
#include "scripts/iosys.h"

// Angelscript Support
#if defined( SCRIPT_ANGELSCRIPT )
#include "scripts/angelscript.h"
#endif

namespace ScriptSystem
{
    // Our available scripts to register to
    enum Scripts
    {
        InputOutput = 0,
        Angelscript,
    };
    using pOnScriptCallback = void (*)(CScriptCallback *pData);

    // Calls a script function
    int CallScript( Scripts nType, pOnScriptCallback pCallback, std::string szFunctionName, ... );

    // Register a default callback for our C++ code
    void RegisterScriptCallback( Scripts nType, pOnScriptCallback pCallback, std::string szFunctionName );
}

#endif
