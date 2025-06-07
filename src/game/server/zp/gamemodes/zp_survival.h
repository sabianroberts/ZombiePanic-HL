// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef SERVER_GAMEMODE_SURVIVAL
#define SERVER_GAMEMODE_SURVIVAL
#pragma once

class ZPGameMode_Survival : public CBaseGameMode
{
	SET_BASECLASS( CBaseGameMode );
public:
	ZPGameMode_Survival();

protected:
	virtual void OnHUDInit(CBasePlayer *pPlayer);
	virtual void OnGameModeThink();
	virtual void GetZombieLifeData( int &current, int &max ) override;
	virtual void OnZombieLifeUpdated( bool bIncreased ) override;
	virtual void OnPlayerDied( CBasePlayer *pPlayer, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void OnPlayerSpawned( CBasePlayer *pPlayer );
	virtual WinState_e GetWinState();
	virtual void RestartRound();
	bool HasNoRemainingZombies() const;

	virtual void UpdateZombieLifesForClient();
	virtual void GiveWeaponsOnRoundStart();

private:
	int m_iZombieLives;
};

#endif