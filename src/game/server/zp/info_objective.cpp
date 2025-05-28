// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "eiface.h"
#include "zp/gamemodes/zp_gamemodebase.h"
#include "zp/info_objective.h"

// Our actual objective entity
LINK_ENTITY_TO_CLASS( info_objective, CObjectiveMessage );

// Should only be used for applying the objective state
LINK_ENTITY_TO_CLASS( info_objective_set, CObjectiveMessageSet );

extern int gmsgObjective;

void CObjectiveMessage::Spawn( void )
{
	m_State = ObjectiveState::State_Normal;
	UTIL_strcpy( m_Message, "My Objective" );
	m_NextObj[0] = 0;
	BaseClass::Spawn();
}

void CObjectiveMessage::Restart()
{
	m_State = ObjectiveState::State_Normal;
}

void CObjectiveMessage::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "message" ) )
	{
		UTIL_strcpy( m_Message, pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "next_obj" ) )
	{
		UTIL_strcpy( m_NextObj, pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		BaseClass::KeyValue( pkvd );
}

void CObjectiveMessage::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// If we are on this state, then refuse any changes after.
	if ( m_State >= State_Completed ) return;

	// Since GoldSrc does not have the I/O from Source, we simply use the "value" for
	// applying our objective state.
	int iValue = (int)clamp( value, ObjectiveState::State_Normal, ObjectiveState::State_Failed );
	switch ( iValue )
	{
		case 0: m_State = State_Normal; break;
		case 1: m_State = State_InProgress; break;
		case 2: m_State = State_Completed; break;
		case 3: m_State = State_Failed; break;
	}

	switch ( m_State )
	{
		// If completed, then fire the next objective after 4 seconds.
		case State_Completed:
		{
			SetThink( &CObjectiveMessage::CallNewObjective );
		    pev->nextthink = gpGlobals->time + 1.0f;
		}
		break;
	}
	UpdateMessageState();
}

void CObjectiveMessage::UpdateMessageState()
{
	// Make sure we are on the right gamemode
	ZP::GameModeType_e eGameModeType = ZP::IsValidGameModeMap( STRING( gpGlobals->mapname ) );
	if ( eGameModeType != ZP::GAMEMODE_OBJECTIVE ) return;

	// Send our info to the client.
	// All they care about is our state and message.
	// They do not care about your feelings.
	MESSAGE_BEGIN( MSG_ALL, gmsgObjective );
	WRITE_BYTE( m_State );
	WRITE_STRING( m_Message );
	MESSAGE_END();
}

void CObjectiveMessage::CallNewObjective()
{
	// Stop thinking
	SetThink( NULL );

	// Make sure we are on the right gamemode
	ZP::GameModeType_e eGameModeType = ZP::IsValidGameModeMap( STRING( gpGlobals->mapname ) );
	if ( eGameModeType != ZP::GAMEMODE_OBJECTIVE ) return;

	// Let's search for our new message
	CBaseEntity *pFind = UTIL_FindEntityByTargetname( nullptr, m_NextObj );
	if ( !pFind ) return;
	pFind->Use( nullptr, nullptr, USE_ON, 0 );
}

bool CObjectiveMessage::IsFirstObjective()
{
	if ( pev->spawnflags & OBJECTIVE_MESSAGE_FIRST ) return true;
	return false;
}

//-----------------------------------------------------------------------------------

void CObjectiveMessageSet::KeyValue(KeyValueData *pkvd)
{
	if ( FStrEq( pkvd->szKeyName, "objstate" ) )
	{
		m_State = (ObjectiveState)atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		BaseClass::KeyValue( pkvd );
}

void CObjectiveMessageSet::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBaseEntity *pFind = UTIL_FindEntityByTargetname( nullptr, STRING(pev->target) );
	if ( !pFind ) return;
	pFind->Use( nullptr, nullptr, USE_ON, m_State );
}
