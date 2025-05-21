
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "zp/zp_shared.h"

#include "zp_gamemodebase.h"
#include "zp_objective.h"

extern cvar_t timeleft;

ZPGameMode_Objective::ZPGameMode_Objective()
{
	SetRoundState( ZP::RoundState_WaitingForPlayers );
	m_bTimeRanOut = false;
	m_bAllSurvivorsDead = false;
	m_bHasPlayersReachedEnd = false;
	m_flRoundBeginsIn = 0;
}

void ZPGameMode_Objective::OnHUDInit(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgRoundState, NULL, pPlayer->edict());
	WRITE_SHORT(GetRoundState());
	MESSAGE_END();
}

void ZPGameMode_Objective::OnGameModeThink()
{
	// Check who is alive (survivors only)
	bool bHasSomeoneAlive = false;
	int iSurvivorsFound = 0;
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );
		if ( plr )
		{
			int iTeam = plr->pev->team;
			if ( iTeam == ZP::TEAM_SURVIVIOR )
			{
				if ( GetRoundState() == ZP::RoundState_WaitingForPlayers )
					iSurvivorsFound++;
				else if ( plr->IsAlive() && !bHasSomeoneAlive )
					bHasSomeoneAlive = true;
			}
		}
	}

	switch ( GetRoundState() )
	{
		case ZP::RoundState_WaitingForPlayers:
		{
			if ( iSurvivorsFound >= 2 )
			{
				m_flRoundBeginsIn = gpGlobals->time + 5;
				SetRoundState( ZP::RoundState_RoundIsStarting );
				MESSAGE_BEGIN(MSG_ALL, gmsgRoundState);
				WRITE_SHORT(GetRoundState());
				MESSAGE_END();
			}
		}
		break;

		case ZP::RoundState_RoundIsStarting:
		{
			float flRoundBeginsIn = m_flRoundBeginsIn - gpGlobals->time;
			if ( flRoundBeginsIn <= 0 )
			{
				m_flRoundBeginsIn = 0;
				SetRoundState( ZP::RoundState_PickVolunteers );
			}
		}
	    break;

		case ZP::RoundState_PickVolunteers:
		{
			SetRoundState( ZP::RoundState_RoundHasBegunPost );
		}
	    break;

		case ZP::RoundState_RoundHasBegunPost:
		{
			SetRoundState( ZP::RoundState_RoundHasBegun );
		    MESSAGE_BEGIN(MSG_ALL, gmsgRoundState);
		    WRITE_SHORT(GetRoundState());
		    MESSAGE_END();
		    GiveWeaponsOnRoundStart();
		}
	    break;
	}

	// If the round has not yet begun, we do not care about the timer
	if ( GetRoundState() != ZP::RoundState_RoundHasBegun ) return;

	static int last_time;

	int time_remaining = 0;

	float flTimeLimit = CVAR_GET_FLOAT( "mp_timelimit" ) * 60;
	time_remaining = (int)(flTimeLimit ? (flTimeLimit - gpGlobals->time) : 0);
	if ( flTimeLimit != 0 && gpGlobals->time >= flTimeLimit )
		m_bTimeRanOut = true;

	// Updates once per second
	if (timeleft.value != last_time)
		g_engfuncs.pfnCvar_DirectSet(&timeleft, UTIL_VarArgs("%i", time_remaining));
	last_time = time_remaining;

	// If we did not find anyone, then they may be all be dead (or disconnected)
	if ( !bHasSomeoneAlive )
		m_bAllSurvivorsDead = true;
}

void ZPGameMode_Objective::OnPlayerDied( CBasePlayer *pPlayer, entvars_t *pKiller, entvars_t *pInflictor )
{
}

void ZPGameMode_Objective::OnPlayerSpawned( CBasePlayer *pPlayer )
{
}

ZPGameMode_Objective::WinState_e ZPGameMode_Objective::GetWinState()
{
	if ( m_bTimeRanOut ) return State_ZombieWin;
	if ( m_bHasPlayersReachedEnd ) return State_SurvivorWin;
	return m_bAllSurvivorsDead ? State_ZombieWin : State_None;
}

void ZPGameMode_Objective::GiveWeaponsOnRoundStart()
{
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );
		if ( plr && plr->IsAlive() )
		{
			int iTeam = plr->pev->team;
			if ( iTeam == ZP::TEAM_SURVIVIOR )
				GiveWeapons( (CBasePlayer *)plr );
		}
	}
}

void ZPGameMode_Objective::GiveWeapons(CBasePlayer *pPlayer)
{
	CBaseEntity *pWeaponEntity = NULL;
	BOOL addDefault = TRUE;

	while (pWeaponEntity = UTIL_FindEntityByClassname(pWeaponEntity, "game_player_equip"))
	{
		pWeaponEntity->Touch(pPlayer);
		addDefault = FALSE;
	}

	if (addDefault)
	{
		pPlayer->GiveNamedItem( "weapon_crowbar" );
		pPlayer->GiveNamedItem( "weapon_9mmhandgun" );
		pPlayer->GiveAmmo( 68, "9mm", _9MM_MAX_CARRY ); // 4 full reloads
	}

	pPlayer->m_iHideHUD = 0;
}

