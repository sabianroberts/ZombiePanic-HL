// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef SCRIPT_SYSTEM_SCRIPT_CALLBACK
#define SCRIPT_SYSTEM_SCRIPT_CALLBACK
#pragma once

#include <string>   //std::string
#include <vector>   //std::vector

class CScriptCallback
{
private:
    // Use KeyValues instead?
    struct script_data
    {
        std::string Name;
    };
    std::vector<script_data> m_ScriptData;
    
public:
    CScriptCallback();
    ~CScriptCallback();
};

CScriptCallback::CScriptCallback()
{
}

CScriptCallback::~CScriptCallback()
{
}


#endif
