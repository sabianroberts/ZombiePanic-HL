#include "zp_gamemodebase.h"

static IGameModeBase *s_GameModeBase = nullptr;

IGameModeBase *ZP::GetCurrentGameMode()
{
	return s_GameModeBase;
}

void ZP::SetCurrentGameMode( IGameModeBase *pGameMode )
{
	if ( s_GameModeBase )
		delete s_GameModeBase;
	s_GameModeBase = pGameMode;
}
