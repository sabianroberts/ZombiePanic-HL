#ifndef SERVER_GAMEMODE_OBJECTIVE
#define SERVER_GAMEMODE_OBJECTIVE
#pragma once

class ZPGameMode_Objective : public IGameModeBase
{
public:
	ZPGameMode_Objective();

protected:
	virtual void OnHUDInit(CBasePlayer *pPlayer);
	virtual void OnGameModeThink();
	virtual void OnPlayerDied( CBasePlayer *pPlayer, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void OnPlayerSpawned( CBasePlayer *pPlayer );
	virtual WinState_e GetWinState();
	virtual void SetWinState(WinState_e state) override
	{
		switch ( state )
		{
			case IGameModeBase::State_Draw:
			case IGameModeBase::State_ZombieWin: m_bTimeRanOut = true; break;
			case IGameModeBase::State_SurvivorWin: m_bHasPlayersReachedEnd = true; break;
		}
	}

	virtual void GiveWeaponsOnRoundStart();
	virtual void GiveWeapons(CBasePlayer *pPlayer);

private:
	float m_flRoundBeginsIn;
	bool m_bTimeRanOut;
	bool m_bAllSurvivorsDead;
	bool m_bHasPlayersReachedEnd;
};

#endif