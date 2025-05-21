
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "zp/zp_shared.h"

#include "zp_gamemodebase.h"
#include "zp_survival.h"

extern cvar_t timeleft;

#define MAX_ZOMBIE_LIVES 99

ZPGameMode_Survival::ZPGameMode_Survival()
{
	SetRoundState( ZP::RoundState_WaitingForPlayers );
	m_bTimeRanOut = false;
	m_bAllSurvivorsDead = false;
	m_iZombieLives = 5;
	m_flRoundBeginsIn = 0;
}

void ZPGameMode_Survival::OnHUDInit(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgRoundState, NULL, pPlayer->edict());
	WRITE_SHORT(GetRoundState());
	MESSAGE_END();
}

void ZPGameMode_Survival::OnGameModeThink()
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

void ZPGameMode_Survival::GetZombieLifeData( int &current, int &max )
{
	current = m_iZombieLives;
	max = MAX_ZOMBIE_LIVES;
}

void ZPGameMode_Survival::OnZombieLifeUpdated( bool bIncreased )
{
	int iUpdate = m_iZombieLives;
	if ( bIncreased )
		++iUpdate;
	else
		--iUpdate;
	m_iZombieLives = (int)clamp( iUpdate, 0, MAX_ZOMBIE_LIVES );

	// Make sure we tell the client about this!
	UpdateZombieLifesForClient();
}

void ZPGameMode_Survival::OnPlayerDied( CBasePlayer *pPlayer, entvars_t *pKiller, entvars_t *pInflictor )
{
	int iTeam = pPlayer->pev->team;
	if ( iTeam == ZP::TEAM_ZOMBIE )
		OnZombieLifeUpdated( false );
	else if ( iTeam == ZP::TEAM_SURVIVIOR )
		OnZombieLifeUpdated( true );
	if ( m_iZombieLives <= 0 )
		pPlayer->StartObserver();
}

void ZPGameMode_Survival::OnPlayerSpawned( CBasePlayer *pPlayer )
{
	// TODO: Check if we spawned as a volunteer
	// m_iZombieLives++
}

ZPGameMode_Survival::WinState_e ZPGameMode_Survival::GetWinState()
{
	if ( m_bTimeRanOut ) return State_SurvivorWin;
	if ( m_iZombieLives <= 0 ) return State_SurvivorWin;
	return m_bAllSurvivorsDead ? State_ZombieWin : State_None;
}

void ZPGameMode_Survival::UpdateZombieLifesForClient()
{
	MESSAGE_BEGIN(MSG_ALL, gmsgZombieLives);
	WRITE_SHORT(m_iZombieLives);
	MESSAGE_END();
}

void ZPGameMode_Survival::GiveWeaponsOnRoundStart()
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

void ZPGameMode_Survival::GiveWeapons(CBasePlayer *pPlayer)
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

