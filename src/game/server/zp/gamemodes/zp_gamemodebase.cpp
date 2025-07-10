// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"

#include "zp/info_random_base.h"
#include "zp_gamemodebase.h"

static IGameModeBase *s_GameModeBase = nullptr;

IGameModeBase *ZP::GetCurrentGameMode()
{
	return s_GameModeBase;
}

void ZP::SetCurrentGameMode( IGameModeBase *pGameMode )
{
	if ( s_GameModeBase )
		delete s_GameModeBase;
	s_GameModeBase = pGameMode;
}

ZP::RoundState ZP::GetCurrentRoundState()
{
	IGameModeBase *pGameMode = GetCurrentGameMode();
	if ( !pGameMode ) return ZP::RoundState_Invalid;
	return pGameMode->GetRoundState();
}

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

extern cvar_t roundtime;
extern cvar_t testmode;
extern cvar_t startdelay;
extern int gmsgTeamInfo;

bool CBaseGameMode::IsTestModeActive() const
{
	return (testmode.value >= 1) ? true : false;
}

void CBaseGameMode::OnHUDInit(CBasePlayer *pPlayer)
{
	if ( !m_bHasPlayersConnected )
		m_bHasPlayersConnected = true;
}

void CBaseGameMode::OnGameModeThink()
{
	// Nobody has connected, don't do any thinking.
	if ( !m_bHasPlayersConnected ) return;

	// Check who is alive (survivors only)
	bool bHasSomeoneAlive = false;
	int iSurvivorsFound = 0;
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *plr = (CBasePlayer *)UTIL_PlayerByIndex( i );
		if ( plr && plr->IsConnected() )
		{
			int iTeam = plr->pev->team;
			if ( iTeam == ZP::TEAM_SURVIVIOR && plr->IsAlive() )
			{
				if ( GetRoundState() == ZP::RoundState_WaitingForPlayers )
					iSurvivorsFound++;
				else if ( !bHasSomeoneAlive )
					bHasSomeoneAlive = true;
			}
		}
	}

	switch ( GetRoundState() )
	{
		case ZP::RoundState_WaitingForPlayers:
		{
			if ( iSurvivorsFound >= 2 || IsTestModeActive() )
			{
			    m_flRoundBeginsIn = gpGlobals->time + startdelay.value;
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
			    m_flRoundBeginsIn = gpGlobals->time + 0.1f;
				SetRoundState( ZP::RoundState_PickVolunteers );
			}
		}
	    break;

		case ZP::RoundState_PickVolunteers:
		{
			float flRoundBeginsIn = m_flRoundBeginsIn - gpGlobals->time;
			if ( flRoundBeginsIn <= 0 )
			{
			    m_flRoundBeginsIn = 0;
				SetRoundState( ZP::RoundState_RoundHasBegunPost );
			}
		}
	    break;

		case ZP::RoundState_RoundHasBegunPost:
	    {
		    m_flRoundTime = roundtime.value > 0 ? gpGlobals->time + roundtime.value * 60 : -1;
		    m_flLastZombieCheck = gpGlobals->time + 4.5f;
			SetRoundState( ZP::RoundState_RoundHasBegun );
		    MESSAGE_BEGIN(MSG_ALL, gmsgRoundState);
		    WRITE_SHORT(GetRoundState());
		    MESSAGE_END();
		    GiveWeaponsOnRoundStart();
		    ZP::SpawnWeaponsFromRandomEntities();
		}
	    break;
	}

	// Function to fire and used by gamemodes
	OnRoundStateThink( GetRoundState() );

	// If the round has not yet begun, we do not care about the timer
	if ( GetRoundState() != ZP::RoundState_RoundHasBegun ) return;

	// Sorry pal, we don't care if everyone is dead, or the timeleft inf testmode.
	if ( IsTestModeActive() ) return;

	CheckZombieAmount();

	// Game time is over, go to intermission
	if ( m_flRoundTime != -1 && gpGlobals->time >= m_flRoundTime )
		m_bTimeRanOut = true;

	// If we did not find anyone, then they may be all be dead (or disconnected)
	if ( !bHasSomeoneAlive )
		m_bAllSurvivorsDead = true;
}

void CBaseGameMode::GiveWeaponsOnRoundStart()
{
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *plr = (CBasePlayer *)UTIL_PlayerByIndex( i );
		if ( plr && plr->IsAlive() )
		{
			int iTeam = plr->pev->team;
			if ( iTeam == ZP::TEAM_SURVIVIOR )
				GiveWeapons( plr );
			plr->m_iHideHUD = 0;
			plr->m_flCanSuicide = gpGlobals->time + 20.0f;
			plr->m_flSuicideTimer = -1; // Just in case if the player manages to frame perfect a "kill" command.
			// Zombies can't choose weapons, they only got their arms.
			if ( iTeam == ZP::TEAM_ZOMBIE )
				plr->m_iHideHUD |= HIDEHUD_WEAPONS;
		}
	}
}

void CBaseGameMode::RestartRound()
{
	SetRoundState( ZP::RoundState::RoundState_WaitingForPlayers );
	SetWinState( WinState_e::State_None );
	m_bAllSurvivorsDead = false;
	m_bTimeRanOut = false;
	MESSAGE_BEGIN( MSG_ALL, gmsgRoundState );
	WRITE_SHORT( GetRoundState() );
	MESSAGE_END();
}

void CBaseGameMode::CheckZombieAmount()
{
	if ( m_flLastZombieCheck - gpGlobals->time > 0 ) return;
	m_flLastZombieCheck = gpGlobals->time + 3.0f;
	int iZombies = 0;
	std::vector<int> m_Volunteers;
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *plr = (CBasePlayer *)UTIL_PlayerByIndex( i );
		if ( plr )
		{
			int iTeam = plr->pev->team;
			if ( iTeam == ZP::TEAM_ZOMBIE )
				iZombies++;
			else if ( iTeam == ZP::TEAM_SURVIVIOR )
				m_Volunteers.push_back( plr->entindex() );
		}
	}
	// Check if we have any zombies
	if ( iZombies > 0 ) return;
	// We got no zombies? Pick a random survivor to become a zombie on the spot.
	int iVolunteerIndex = 0;
	int iVolunteers = m_Volunteers.size() - 1;
	if ( iVolunteers > 0 )
		iVolunteerIndex = RANDOM_LONG( 0, iVolunteers );
	int iPlayerIndex = m_Volunteers[iVolunteerIndex];

	if ( iPlayerIndex == 0 ) return;
	m_Volunteers.erase( m_Volunteers.begin() + iVolunteerIndex );
	CBasePlayer *plr = (CBasePlayer *)UTIL_PlayerByIndex( iPlayerIndex );
	if ( plr )
	{
		// Drop everything in a backpack.
		plr->m_flLastPanic = gpGlobals->time + 30;
		plr->m_flPanicTime = gpGlobals->time + 3.5;
		plr->PackDeadPlayerItems();
		plr->RemoveAllItems( FALSE );

		// Change team
		plr->pev->team = ZP::TEAM_ZOMBIE;

		// notify everyone's HUD of the team change
		MESSAGE_BEGIN( MSG_ALL, gmsgTeamInfo );
		WRITE_BYTE( iPlayerIndex );
		WRITE_STRING( plr->pev->iuser1 ? "" : plr->TeamID() );
		MESSAGE_END();

		plr->SendScoreInfo();
		plr->SpawnInPlace( true );
		plr->Spawn();
	}
}

void CBaseGameMode::GiveWeapons( CBasePlayer *pPlayer )
{
	CBaseEntity *pWeaponEntity = NULL;
	BOOL addDefault = TRUE;

	while (pWeaponEntity = UTIL_FindEntityByClassname(pWeaponEntity, "game_player_equip"))
	{
		pWeaponEntity->Touch( pPlayer );
		addDefault = FALSE;
	}

	if (addDefault)
	{
		pPlayer->GiveNamedItem( "weapon_crowbar" );
		pPlayer->GiveNamedItem( "weapon_sig" );
		pPlayer->GiveAmmo( 14, ZPAmmoTypes::AMMO_PISTOL );
	}
}

bool CBaseGameMode::WasAlreadyChoosenPreviously( CBasePlayer *pPlayer, bool bVerifyOnly )
{
	const char *authId = GETPLAYERAUTHID( pPlayer->edict() );
	if ( authId )
	{
		for ( size_t i = 0; i < m_LastChoosenZombies.size(); i++ )
		{
			std::string szAuthCheck = m_LastChoosenZombies[i];
			if ( FStrEq( szAuthCheck.c_str(), authId ) )
				return true;
		}
		if ( !bVerifyOnly )
			m_LastChoosenZombies.push_back( authId );
	}
	return false;
}

void CBaseGameMode::ShouldClearChoosenZombies()
{
	// How many players do we have? If everyone was choosen, purge the list.
	int iAmountNotChoosen = 0;
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *plr = (CBasePlayer *)UTIL_PlayerByIndex( i );
		if ( plr && !WasAlreadyChoosenPreviously( plr, true ) )
			iAmountNotChoosen++;
	}

	// There are no left
	if ( iAmountNotChoosen == 0 )
		m_LastChoosenZombies.clear();
}
