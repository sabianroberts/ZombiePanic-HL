// ============== Copyright (c) 2025 Monochrome Games ============== \\

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

ZPGameMode_Objective::WinState_e ZPGameMode_Objective::GetWinState()
{
	if ( m_bTimeRanOut ) return State_ZombieWin;
	if ( m_bHasPlayersReachedEnd ) return State_SurvivorWin;
	return m_bAllSurvivorsDead ? State_ZombieWin : State_None;
}

