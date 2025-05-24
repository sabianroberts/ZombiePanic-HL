// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef SERVER_GAMEMODE_DEVMODE
#define SERVER_GAMEMODE_DEVMODE
#pragma once

class ZPGameMode_Dev : public IGameModeBase
{
public:
	ZPGameMode_Dev();

protected:
	virtual bool IsTestModeActive() const { return false; };
	virtual void OnHUDInit(CBasePlayer *pPlayer);
	virtual void OnGameModeThink() {}
	virtual void RestartRound() { }
	virtual void OnPlayerDied( CBasePlayer *pPlayer, entvars_t *pKiller, entvars_t *pInflictor ) {}
	virtual void OnPlayerSpawned( CBasePlayer *pPlayer ) {}
	virtual WinState_e GetWinState() { return WinState_e::State_None; }
};

#endif