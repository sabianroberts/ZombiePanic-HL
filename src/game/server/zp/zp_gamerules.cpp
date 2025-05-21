
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include "zp_gamerules.h"
#include "game.h"

extern DLL_GLOBAL BOOL g_fGameOver;

CZombiePanicGameRules::CZombiePanicGameRules()
{
	m_DisableDeathMessages = FALSE;
	m_DisableDeathPenalty = FALSE;
	m_bHasPickedVolunteer = false;
	m_Volunteers.clear();
	m_Rejoiners.clear();
	m_GameModeType = ZP::IsValidGameModeMap( STRING( gpGlobals->mapname ) );
	switch ( m_GameModeType )
	{
		case ZP::GAMEMODE_SURVIVAL: m_pGameMode = new ZPGameMode_Survival; break;
		case ZP::GAMEMODE_OBJECTIVE: m_pGameMode = new ZPGameMode_Objective; break;
		default: m_pGameMode = new ZPGameMode_Dev; break;
	}
	ZP::SetCurrentGameMode( m_pGameMode );
}

CZombiePanicGameRules::~CZombiePanicGameRules()
{
	ZP::SetCurrentGameMode( nullptr );
}

#include "voice_gamemgr.h"
extern CVoiceGameMgr g_VoiceGameMgr;

void CZombiePanicGameRules ::Think(void)
{
	CGameRules::Think();

	g_VoiceGameMgr.Update(gpGlobals->frametime);

	if ( g_fGameOver ) // someone else quit the game already
	{
		BaseClass::Think();
		return;
	}

	IGameModeBase::WinState_e eWinState = m_pGameMode->GetWinState();
	if ( eWinState >= IGameModeBase::WinState_e::State_Draw )
	{
		GoToIntermission();
		return;
	}

	// We do the gamemode thinking on a seperate class, to make it less
	// of a mess.
	m_pGameMode->OnGameModeThink();

	switch ( m_pGameMode->GetRoundState() )
	{
		case ZP::RoundState::RoundState_WaitingForPlayers: ResetVolunteers(); break;
		case ZP::RoundState::RoundState_PickVolunteers: PickRandomVolunteer(); break;
	}

#if 0
	float flFragLimit = fraglimit.value;
	if (flFragLimit)
	{
		int bestfrags = 9999;
		int remain;

		// check if any team is over the frag limit
		for (int i = 0; i < ZP::MAX_TEAM; i++)
		{
			if (ZP::Teams[i] >= flFragLimit)
			{
				GoToIntermission();
				return;
			}

			remain = flFragLimit - team_scores[i];
			if (remain < bestfrags)
			{
				bestfrags = remain;
			}
		}
		frags_remaining = bestfrags;
	}

	// Updates when frags change
	if (frags_remaining != last_frags)
	{
		g_engfuncs.pfnCvar_DirectSet(&fragsleft, UTIL_VarArgs("%i", frags_remaining));
	}

	last_frags = frags_remaining;
#endif
}

extern int gmsgGameMode;
extern int gmsgSayText;
extern int gmsgTeamInfo;
extern int gmsgTeamNames;
extern int gmsgScoreInfo;

void CZombiePanicGameRules::UpdateGameMode(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, NULL, pPlayer->edict());
	WRITE_BYTE(m_GameModeType);
	MESSAGE_END();
}

const char *CZombiePanicGameRules::SetDefaultPlayerTeam(CBasePlayer *pPlayer)
{
	return pPlayer->TeamID();
}

//=========================================================
// InitHUD
//=========================================================
void CZombiePanicGameRules::InitHUD(CBasePlayer *pPlayer)
{
	BaseClass::InitHUD(pPlayer);

	// update the current player of the team he is joining
	char text[256];

	// Send down the team names
	MESSAGE_BEGIN(MSG_ONE, gmsgTeamNames, NULL, pPlayer->edict());
	WRITE_BYTE(ZP::MAX_TEAM);
	for (int i = 0; i < ZP::MAX_TEAM; i++)
	{
		WRITE_STRING(ZP::Teams[i]);
	}
	MESSAGE_END();

	ChangePlayerTeam(pPlayer, ZP::Teams[ZP::TEAM_OBSERVER], FALSE, FALSE);

	// loop through all active players and send their team info to the new client
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		CBaseEntity *plr = UTIL_PlayerByIndex(i);
		if (plr && IsValidTeam(plr->TeamID()))
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgTeamInfo, NULL, pPlayer->edict());
			WRITE_BYTE(plr->entindex());
			WRITE_STRING(plr->pev->iuser1 ? "" : plr->TeamID());
			MESSAGE_END();
		}
	}

	m_pGameMode->OnHUDInit( pPlayer );
}

void CZombiePanicGameRules::ChangePlayerTeam(CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib)
{
	int clientIndex = pPlayer->entindex();

	if (bKill)
	{
		// kill the player,  remove a death,  and let them start on the new team
		m_DisableDeathMessages = TRUE;
		m_DisableDeathPenalty = TRUE;

		int damageFlags = DMG_GENERIC | (bGib ? DMG_ALWAYSGIB : DMG_NEVERGIB);
		entvars_t *pevWorld = VARS(INDEXENT(0));
		pPlayer->TakeDamage(pevWorld, pevWorld, 10000, damageFlags);

		m_DisableDeathMessages = FALSE;
		m_DisableDeathPenalty = FALSE;
	}

	// Set team to player
	pPlayer->pev->team = GetTeamIndex( pTeamName );

	// notify everyone's HUD of the team change
	MESSAGE_BEGIN(MSG_ALL, gmsgTeamInfo);
	WRITE_BYTE(clientIndex);
	WRITE_STRING(pPlayer->pev->iuser1 ? "" : pPlayer->TeamID());
	MESSAGE_END();

	pPlayer->SendScoreInfo();
}

void CZombiePanicGameRules::ResetVolunteers()
{
	if ( !m_bHasPickedVolunteer ) return;
	m_bHasPickedVolunteer = false;
	m_Volunteers.clear();
	m_Rejoiners.clear();
}

void CZombiePanicGameRules::PickRandomVolunteer()
{
	if ( m_bHasPickedVolunteer ) return;
	m_bHasPickedVolunteer = true;
	// We have no volunteers, so lets pick some random survivors
	if ( m_Volunteers.size() == 0 )
	{
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBaseEntity *plr = UTIL_PlayerByIndex(i);
			if ( plr && plr->IsAlive() && plr->pev->team == ZP::TEAM_SURVIVIOR )
				m_Volunteers.push_back( plr->entindex() );
		}
	}

	int iVolunteers = m_Volunteers.size() - 1;
	int iPlayerIndex = 0;
	if ( iVolunteers == 0 )
		iPlayerIndex = m_Volunteers[0];
	else
		iPlayerIndex = rand() % iVolunteers;

	if ( iPlayerIndex == 0 ) return;
	CBasePlayer *plr = (CBasePlayer *)UTIL_PlayerByIndex( iPlayerIndex );
	if ( plr )
	{
		ChangePlayerTeam( plr, ZP::Teams[ZP::TEAM_ZOMBIE], FALSE, FALSE );
		plr->Spawn();
	}
}

void CZombiePanicGameRules::PlayerSpawn(CBasePlayer *pPlayer)
{
	// Start welcome cam for new players
	if (!pPlayer->m_bPutInServer && mp_welcomecam.GetBool() != 0)
	{
		// don't let him spawn as soon as he enters the server
		// give enough time to plugins to send the player to spectator mode
		pPlayer->m_flNextAttack = mp_welcomecam_delay.GetFloat();

		pPlayer->StartWelcomeCam();
		return;
	}

	SetPlayerModel( pPlayer );

	int iTeamNumber = pPlayer->pev->team;
	if ( iTeamNumber == ZP::TEAM_OBSERVER ) return;

	// Player rejoined, force zombie.
	if ( !HasAlreadyJoined( pPlayer ) )
		m_Rejoiners.push_back(pPlayer->entindex() );

	int aws = pPlayer->m_iAutoWepSwitch;
	pPlayer->m_iAutoWepSwitch = 1;

	pPlayer->pev->weapons |= (1 << WEAPON_SUIT);

	// Zombies
	if ( iTeamNumber == ZP::TEAM_ZOMBIE )
	{
		// Zombie arms!
		// That's all that the zombies get!
		pPlayer->GiveNamedItem( "weapon_swipe" );
	}

	FireTargets("game_playerspawn", pPlayer, pPlayer, USE_TOGGLE, 0);

	pPlayer->m_iAutoWepSwitch = aws;

	m_pGameMode->OnPlayerSpawned( pPlayer );
}

BOOL CZombiePanicGameRules::ClientCommand(CBasePlayer *pPlayer, const char *pcmd)
{
	if (FStrEq(pcmd, "joingame"))
	{
		if ( pPlayer->pev->team == ZP::TEAM_OBSERVER )
		{
			const char *pVolunteer = CMD_ARGV(1);
			if ( pVolunteer && pVolunteer[0] )
			{
				if (FStrEq(pcmd, "volunteer"))
					m_Volunteers.push_back( pPlayer->entindex() );
			}
			bool bLateJoin = ( m_pGameMode->GetRoundState() == ZP::RoundState::RoundState_RoundHasBegun ) ? true : false;

			if ( HasAlreadyJoined( pPlayer ) )
				bLateJoin = true;

			ChangePlayerTeam(pPlayer, ZP::Teams[ bLateJoin ? ZP::TEAM_ZOMBIE : ZP::TEAM_SURVIVIOR], FALSE, FALSE);
			pPlayer->StopWelcomeCam();
			if ( m_pGameMode->GetRoundState() < ZP::RoundState::RoundState_RoundHasBegun )
				pPlayer->m_iHideHUD = HIDEHUD_WEAPONS | HIDEHUD_HEALTH | HIDEHUD_FLASHLIGHT;
		}
		return TRUE;
	}
	return BaseClass::ClientCommand(pPlayer, pcmd);
}

bool CZombiePanicGameRules::HasAlreadyJoined(CBasePlayer *pPlayer)
{
	return false;
}

void CZombiePanicGameRules::SetPlayerModel(CBasePlayer *pPlayer)
{
	int iTeam = pPlayer->pev->team;
	g_engfuncs.pfnSetClientKeyValue(
		pPlayer->entindex(),
		g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ),
		"model",
	    iTeam == ZP::TEAM_ZOMBIE ? "undead" : "survivor"
	);

	if ( iTeam == ZP::TEAM_SURVIVIOR )
		pPlayer->pev->maxspeed = 220;
	else
		pPlayer->pev->maxspeed = 250;
}

//=========================================================
// ClientUserInfoChanged
//=========================================================
void CZombiePanicGameRules::ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer)
{
}

extern int gmsgDeathMsg;

//=========================================================
// Deathnotice.
//=========================================================
void CZombiePanicGameRules::DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor)
{
	if (m_DisableDeathMessages)
		return;

	if (pVictim && pKiller && pKiller->flags & FL_CLIENT)
	{
		CBasePlayer *pk = (CBasePlayer *)CBaseEntity::Instance(pKiller);

		if (pk)
		{
			if ((pk != pVictim) && (PlayerRelationship(pVictim, pk) == GR_TEAMMATE))
			{
				MESSAGE_BEGIN(MSG_ALL, gmsgDeathMsg);
				WRITE_BYTE(ENTINDEX(ENT(pKiller))); // the killer
				WRITE_BYTE(ENTINDEX(pVictim->edict())); // the victim
				WRITE_STRING("teammate"); // flag this as a teammate kill
				MESSAGE_END();
				return;
			}
		}
	}

	BaseClass::DeathNotice(pVictim, pKiller, pevInflictor);
}

//=========================================================
//=========================================================
void CZombiePanicGameRules ::PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor)
{
	if (!m_DisableDeathPenalty)
	{
		m_pGameMode->OnPlayerDied( pVictim, pKiller, pInflictor );
		BaseClass::PlayerKilled(pVictim, pKiller, pInflictor);
		if ( pVictim->pev->team == ZP::TEAM_SURVIVIOR )
			ChangePlayerTeam( pVictim, ZP::Teams[ZP::TEAM_ZOMBIE], FALSE, FALSE );
	}
}

void CZombiePanicGameRules::PlayerThink(CBasePlayer *pPlayer)
{
	BaseClass::PlayerThink( pPlayer );
}

//=========================================================
// IsTeamplay
//=========================================================
BOOL CZombiePanicGameRules::IsTeamplay(void)
{
	return TRUE;
}

BOOL CZombiePanicGameRules::FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker)
{
	if (pAttacker && PlayerRelationship(pPlayer, pAttacker) == GR_TEAMMATE)
	{
		// my teammate hit me.
		if ((friendlyfire.value == 0) && (pAttacker != pPlayer))
		{
			// friendly fire is off, and this hit came from someone other than myself,  then don't get hurt
			return FALSE;
		}
	}

	return BaseClass::FPlayerCanTakeDamage(pPlayer, pAttacker);
}

//=========================================================
//=========================================================
int CZombiePanicGameRules::PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget)
{
	// half life multiplay has a simple concept of Player Relationships.
	// you are either on another player's team, or you are not.
	if (!pPlayer || !pTarget || !pPlayer->IsPlayer() || !pTarget->IsPlayer())
		return GR_NOTTEAMMATE;
	// Spectators are teammates, but not players in welcomecam mode
	if (((CBasePlayer *)pPlayer)->IsObserver() && !((CBasePlayer *)pPlayer)->m_bInWelcomeCam && ((CBasePlayer *)pTarget)->IsObserver() && !((CBasePlayer *)pTarget)->m_bInWelcomeCam)
		return GR_TEAMMATE;

	if ((*GetTeamID(pPlayer) != '\0') && (*GetTeamID(pTarget) != '\0') && !_stricmp(GetTeamID(pPlayer), GetTeamID(pTarget)))
	{
		return GR_TEAMMATE;
	}

	return GR_NOTTEAMMATE;
}

//=========================================================
//=========================================================
BOOL CZombiePanicGameRules::ShouldAutoAim(CBasePlayer *pPlayer, edict_t *target)
{
	// always autoaim, unless target is a teammate
	CBaseEntity *pTgt = CBaseEntity::Instance(target);
	if (pTgt && pTgt->IsPlayer())
	{
		if (PlayerRelationship(pPlayer, pTgt) == GR_TEAMMATE)
			return FALSE; // don't autoaim at teammates
	}

	return BaseClass::ShouldAutoAim(pPlayer, target);
}

//=========================================================
//=========================================================
int CZombiePanicGameRules::IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled)
{
	if (!pKilled)
		return 0;

	if (!pAttacker)
		return 1;

	if (pAttacker != pKilled && PlayerRelationship(pAttacker, pKilled) == GR_TEAMMATE)
		return -1;

	return 1;
}

//=========================================================
//=========================================================
const char *CZombiePanicGameRules::GetTeamID(CBaseEntity *pEntity)
{
	if (pEntity == NULL || pEntity->pev == NULL)
		return "";

	// return their team name
	return pEntity->TeamID();
}

int CZombiePanicGameRules::GetTeamIndex(const char *pTeamName)
{
	if (pTeamName && *pTeamName != 0)
	{
		// try to find existing team
		for (int tm = 0; tm < ZP::MAX_TEAM; tm++)
		{
			if (!_stricmp(ZP::Teams[tm], pTeamName))
				return tm;
		}
	}

	return -1; // No match
}

const char *CZombiePanicGameRules::GetIndexedTeamName(int teamIndex)
{
	if (teamIndex < 0 || teamIndex >= ZP::MAX_TEAM)
		return ZP::Teams[0];
	return ZP::Teams[teamIndex];
}

BOOL CZombiePanicGameRules::IsValidTeam(const char *pTeamName)
{
	return (GetTeamIndex(pTeamName) != -1) ? TRUE : FALSE;
}
