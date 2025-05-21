
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "zp/zp_shared.h"

#include "zp_gamemodebase.h"
#include "zp_survival.h"

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
	BaseClass::OnGameModeThink();
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
	BaseClass::GiveWeaponsOnRoundStart();
	// Make sure we update this!
	UpdateZombieLifesForClient();
}
