// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef SERVER_ZOMBIEPANIC_GAMERULES
#define SERVER_ZOMBIEPANIC_GAMERULES
#pragma once

#include "cdll_dll.h"
#include "zp/zp_shared.h"
#include "zp/gamemodes/zp_gamemodebase.h"
#include "zp/gamemodes/zp_dev.h"
#include "zp/gamemodes/zp_survival.h"
#include "zp/gamemodes/zp_objective.h"

class CZombiePanicGameRules : public CHalfLifeMultiplay
{
	SET_BASECLASS( CHalfLifeMultiplay );
public:
	CZombiePanicGameRules();
	~CZombiePanicGameRules();

	//virtual int ItemShouldRespawn(CItem *pItem) { return FALSE; }
	virtual void ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer);
	virtual BOOL IsTeamplay(void);
	virtual BOOL FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker);
	virtual int PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget);
	virtual const char *GetTeamID(CBaseEntity *pEntity);
	virtual BOOL ShouldAutoAim(CBasePlayer *pPlayer, edict_t *target);
	virtual int IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled);
	virtual void InitHUD(CBasePlayer *pl);
	virtual void DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor);
	virtual const char *GetGameDescription(void) { return "Zombie Panic! v1.1"; } // this is the game name that gets seen in the server browser
	virtual void UpdateGameMode(CBasePlayer *pPlayer); // the client needs to be informed of the current game mode
	virtual void PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor);
	virtual void PlayerThink(CBasePlayer *pPlayer);
	virtual void Think(void);
	virtual int GetTeamIndex(const char *pTeamName);
	virtual const char *GetIndexedTeamName(int teamIndex);
	virtual BOOL IsValidTeam(const char *pTeamName);
	const char *SetDefaultPlayerTeam(CBasePlayer *pPlayer);
	virtual void ChangePlayerTeam(CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib);
	virtual float FlPlayerFallDamage(CBasePlayer *pPlayer);

	virtual void ResetRound();
	virtual void CleanUpMap();

	virtual void ResetVolunteers();
	virtual void PickRandomVolunteer();

	virtual void PlayerSpawn(CBasePlayer *pPlayer);
	virtual BOOL ClientCommand(CBasePlayer *pPlayer, const char *pcmd);

private:
	void SetPlayerModel(CBasePlayer *pPlayer);

	BOOL m_DisableDeathMessages;
	BOOL m_DisableDeathPenalty;

	bool m_bHasPickedVolunteer;
	ZP::GameModeType_e m_GameModeType;
	IGameModeBase *m_pGameMode;
	std::vector<int> m_Volunteers;
	float m_flRoundRestartDelay;
	int m_iRounds;
};

#endif