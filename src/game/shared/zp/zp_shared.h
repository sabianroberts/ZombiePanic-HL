#ifndef SHARED_ZOMBIEPANIC
#define SHARED_ZOMBIEPANIC
#pragma once

namespace ZP
{
	enum RoundState
	{
		RoundState_Invalid = -1,
		RoundState_WaitingForPlayers = 0,
		RoundState_RoundIsStarting,
		RoundState_PickVolunteers,
		RoundState_RoundHasBegunPost,
		RoundState_RoundHasBegun,
		RoundState_RoundIsOver
	};

	enum Teams_e
	{
		TEAM_NONE,
		TEAM_OBSERVER,
		TEAM_SURVIVIOR,
		TEAM_ZOMBIE,

		MAX_TEAM
	};

	static int MaxSpeeds[2] = {
		220,	// Human Max Speed
		250		// Zombie Max Speed
	};

	static const char *Teams[MAX_TEAM] = {
	    "",
	    "Observer",
	    "Survivor",
	    "Zombie"
    };

	enum GameModeType_e
    {
		GAMEMODE_INVALID = -1,
		GAMEMODE_NONE,
		GAMEMODE_SURVIVAL,
		GAMEMODE_OBJECTIVE,

		MAX_GAMEMODES
    };
    GameModeType_e IsValidGameModeMap(const char *szLevel);
    static const char *GameModes[MAX_GAMEMODES] = {
	    "Development",
	    "Last Man Standing",
	    "Objective"
    };
}

#endif