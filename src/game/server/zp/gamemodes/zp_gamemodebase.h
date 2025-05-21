#ifndef SERVER_GAMEMODE_BASE
#define SERVER_GAMEMODE_BASE
#pragma once

#include "cdll_dll.h"
#include "zp/zp_shared.h"

extern int gmsgZombieLives;
extern int gmsgRoundState;

class IGameModeBase
{
public:
	enum WinState_e
	{
		State_None,
		State_Draw,
		State_ZombieWin,
		State_SurvivorWin
	};

	virtual void OnHUDInit(CBasePlayer *pPlayer) = 0;
	virtual void GetZombieLifeData( int &current, int &max ) { current = max = 0; };
	virtual void OnZombieLifeUpdated( bool bIncreased ) { };
	virtual void OnGameModeThink() = 0;
	virtual void OnPlayerDied(CBasePlayer *pPlayer, entvars_t *pKiller, entvars_t *pInflictor) = 0;
	virtual void OnPlayerSpawned(CBasePlayer *pPlayer) = 0;
	virtual WinState_e GetWinState() = 0;
	virtual void SetWinState(WinState_e state) { }

	// Roundstate
	void SetRoundState( ZP::RoundState state ) { m_iRoundState = state; }
	ZP::RoundState GetRoundState() const { return m_iRoundState; }

private:
	ZP::RoundState m_iRoundState = ZP::RoundState::RoundState_WaitingForPlayers;
};

namespace ZP
{
	IGameModeBase *GetCurrentGameMode();
	void SetCurrentGameMode( IGameModeBase *pGameMode );
}

#endif