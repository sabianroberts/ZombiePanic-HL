// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "core.h"
#include "iosys.h"

IOSystem::IOSystem()
{
	m_Functions.clear();
	// Make sure we add I/O system into our manager.
	ScriptSystem::AddToScriptManager( this );
}

void IOSystem::OnInit()
{
	// TODO: Add something here?
}

void IOSystem::OnCalled(pOnScriptCallbackReturn pfnCallback, KeyValues *pData, std::string szFunctionName)
{
	if ( !bAvailableToCall ) return;
	for (size_t i = 0; i < m_Functions.size(); i++)
	{
		IScriptFunctions ScriptFunction = m_Functions[i];
		if ( ScriptFunction.Function == szFunctionName )
		{
			if ( ScriptFunction.Callback ) (*ScriptFunction.Callback)(pData);
			if ( ScriptFunction.Entity )
			{
				CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(ScriptFunction.Entity);
				if ( pEntity )
					pEntity->ScriptCallback( pData );
			}
			break;
		}
	}
	if ( pfnCallback )
		(*pfnCallback)(pData, GetScriptType());
}

void IOSystem::OnLevelInit(bool bPostLoad)
{
	if ( !bPostLoad )
		bAvailableToCall = true;
}

void IOSystem::OnLevelShutdown()
{
	bAvailableToCall = false;
	// Clear our functions on shutdown
	m_Functions.clear();
}

void IOSystem::OnRegisterFunction(pOnScriptCallback pCallback, std::string szFunctionName)
{
	if ( FunctionAlreadyExist( szFunctionName ) ) return;
	IScriptFunctions ScriptFunction;
	ScriptFunction.Callback = pCallback;
	ScriptFunction.Entity = nullptr;
	ScriptFunction.Function = szFunctionName;
	m_Functions.push_back( ScriptFunction );
}

void IOSystem::OnRegisterFunction(CBaseEntity *pEntity, std::string szFunctionName)
{
	if ( FunctionAlreadyExist( szFunctionName ) ) return;
	IScriptFunctions ScriptFunction;
	ScriptFunction.Callback = nullptr;
	ScriptFunction.Entity = pEntity->edict();
	ScriptFunction.Function = szFunctionName;
	m_Functions.push_back( ScriptFunction );
}
