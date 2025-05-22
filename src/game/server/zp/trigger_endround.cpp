// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "triggers.h"
#include "zp/gamemodes/zp_gamemodebase.h"

class CTriggerEndRound : public CBaseTrigger
{
public:
	void Spawn(void);
	void OnEndRoundTouch(CBaseEntity *pOther);

private:
	bool m_bActivated;
};
LINK_ENTITY_TO_CLASS( trigger_endround, CTriggerEndRound );

void CTriggerEndRound::Spawn(void)
{
	InitTrigger();
	m_bActivated = false;

	SetTouch( &CTriggerEndRound::OnEndRoundTouch );
}

void CTriggerEndRound::OnEndRoundTouch(CBaseEntity *pOther)
{
	if ( m_bActivated ) return;
	if ( !pOther->IsPlayer() ) return;
	if ( !pOther->IsAlive() ) return;
	if ( pOther->pev->team != ZP::TEAM_SURVIVIOR ) return;
	IGameModeBase *pGameMode = ZP::GetCurrentGameMode();
	if ( !pGameMode ) return;
	m_bActivated = true;
	pGameMode->SetWinState( IGameModeBase::WinState_e::State_SurvivorWin );
}