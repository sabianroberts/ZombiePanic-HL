// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef SCRIPT_SYSTEM_SCRIPT_IOSYSTEM
#define SCRIPT_SYSTEM_SCRIPT_IOSYSTEM
#pragma once

#include "script_interface.h"

class IOSystem : public IBaseScriptClass
{
	IOSystem();

public:
	AvailableScripts_t GetScriptType() { return AvailableScripts_t::InputOutput; }
	void OnInit();
	void OnCalled(pOnScriptCallbackReturn pfnCallback, KeyValues *pData, std::string szFunctionName);
	void OnLevelInit(bool bPostLoad);
	void OnLevelShutdown();
	void OnRegisterFunction(pOnScriptCallback pCallback, std::string szFunctionName);
	void OnRegisterFunction(CBaseEntity *pEntity, std::string szFunctionName);

private:
	inline bool FunctionAlreadyExist(const std::string &szFunctionName)
	{
		for (size_t i = 0; i < m_Functions.size(); i++)
		{
			IScriptFunctions ScriptFunction = m_Functions[i];
			if ( ScriptFunction.Function == szFunctionName )
				return true;
		}
		return false;
	}
	std::vector<IScriptFunctions> m_Functions;
	bool bAvailableToCall = false;
};

#endif
