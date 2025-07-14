// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef SERVER_GAMEMODE_BASE
#define SERVER_GAMEMODE_BASE
#pragma once

#include "cdll_dll.h"
#include "zp/zp_shared.h"

extern int gmsgZombieLives;
extern int gmsgRoundState;
extern int gmsgRoundTime;

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

	virtual bool IsTestModeActive() const = 0;
	virtual void OnHUDInit(CBasePlayer *pPlayer) = 0;
	virtual void GetZombieLifeData( int &current, int &max ) { current = max = 0; };
	virtual void OnZombieLifeUpdated( bool bIncreased ) { };
	virtual void OnGameModeThink() = 0;
	virtual void RestartRound() = 0;
	virtual void OnPlayerDied(CBasePlayer *pPlayer, entvars_t *pKiller, entvars_t *pInflictor) = 0;
	virtual void OnPlayerSpawned(CBasePlayer *pPlayer) = 0;
	virtual WinState_e GetWinState() = 0;
	virtual void SetWinState(WinState_e state) { }
	virtual void GiveWeapons(CBasePlayer *pPlayer) = 0;

	// Roundstate
	void SetRoundState( ZP::RoundState state ) { m_iRoundState = state; }
	ZP::RoundState GetRoundState() const { return m_iRoundState; }
	virtual void OnRoundStateThink( ZP::RoundState state ) {};

	// Was the player already choosen?
	virtual bool WasAlreadyChoosenPreviously( CBasePlayer *pPlayer, bool bVerifyOnly = false ) = 0;
	virtual void ShouldClearChoosenZombies() = 0;

protected:
	float m_flRoundTime = -1;

private:
	ZP::RoundState m_iRoundState = ZP::RoundState::RoundState_WaitingForPlayers;
};

class CBaseGameMode : public IGameModeBase
{
public:
	virtual bool IsTestModeActive() const;
	virtual void OnHUDInit(CBasePlayer *pPlayer);
	virtual void OnGameModeThink();
	virtual void GiveWeaponsOnRoundStart();
	virtual void RestartRound();
	virtual void CheckZombieAmount();
	virtual void GiveWeapons(CBasePlayer *pPlayer);
	virtual void OnPlayerDied( CBasePlayer *pPlayer, entvars_t *pKiller, entvars_t *pInflictor ) {}
	virtual void OnPlayerSpawned( CBasePlayer *pPlayer );
	void UpdateClientTimer();

protected:
	float m_flRoundBeginsIn;
	float m_flLastZombieCheck;
	bool m_bTimeRanOut = false;
	bool m_bAllSurvivorsDead = false;
	bool m_bHasPlayersConnected = false;

	bool WasAlreadyChoosenPreviously( CBasePlayer *pPlayer, bool bVerifyOnly = false );
	void ShouldClearChoosenZombies();

private:
	struct LastChoosenZombie
	{
		// This is an std::string instead of uint64,
		// since the AuthString in GoldSrc can be different
		std::string AuthID;
	};
	std::vector<LastChoosenZombie> m_LastChoosenZombies;
};

namespace ZP
{
	IGameModeBase *GetCurrentGameMode();
	void SetCurrentGameMode( IGameModeBase *pGameMode );
	ZP::RoundState GetCurrentRoundState();
}

#endif