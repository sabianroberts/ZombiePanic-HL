// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef SERVER_RANDOM_ENTITY_BASE
#define SERVER_RANDOM_ENTITY_BASE
#pragma once

#include "cbase.h"
#include "zp/zp_shared.h"

#define OBJECTIVE_MESSAGE_FIRST (1<<0)

class CObjectiveMessage : public CPointEntity
{
	SET_BASECLASS( CPointEntity );

public:
	void Spawn( void );
	void Restart();
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void UpdateMessageState();
	void CallNewObjective();
	bool IsFirstObjective();
	virtual int ObjectCaps(void) { return CBaseEntity::ObjectCaps() | FCAP_MUST_RESET; }

private:
	ObjectiveState m_State;
	char m_Message[32];
	char m_NextObj[32];
};

class CObjectiveMessageSet : public CPointEntity
{
	SET_BASECLASS( CPointEntity );

public:
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

private:
	ObjectiveState m_State = ObjectiveState::State_Normal;
};

#endif