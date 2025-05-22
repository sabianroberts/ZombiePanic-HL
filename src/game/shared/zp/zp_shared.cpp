// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "cbase.h"

#if defined(CLIENT_DLL)
#include "strtools.h"
#endif

#include "weapons.h"
#include "player.h"

#include "zp_shared.h"

ZP::GameModeType_e ZP::IsValidGameModeMap( const char *szLevel )
{
	if ( !szLevel ) return GameModeType_e::GAMEMODE_INVALID;
	char sz[4];
#if defined( CLIENT_DLL )
	char buf[64];
	V_FileBase( szLevel, buf, sizeof(buf) );
	V_strcpy_safe( sz, buf );
#else
	UTIL_strcpy( sz, szLevel );
#endif
	if ( !stricmp( sz, "zp_" ) ) return GameModeType_e::GAMEMODE_SURVIVAL;
	else if ( !stricmp( sz, "zpo" ) ) return GameModeType_e::GAMEMODE_OBJECTIVE;
	return GameModeType_e::GAMEMODE_NONE;
}

float CBasePlayer::GetAmmoWeight( const char *szAmmo )
{
	int ammoindex = GetAmmoIndex( szAmmo );
	int amount = m_rgAmmo[ GetAmmoIndex( szAmmo ) ];
	// These are set via W_Precache, trough UTIL_PrecacheOtherWeapon!
	// It's not like Source Engine, where it has a proper AmmoDef class!!
	// !!! The client uses a copy of this under zp_ammobank.cpp, so if this changes, change that too!
	switch ( ammoindex )
	{
		// Buckshot
		case 1: return amount * AMMOWEIGHT_BUCKSHOT;
		// 9mm
	    case 2: return amount * AMMOWEIGHT_9MM;
		// 556AR
	    case 3: return amount * AMMOWEIGHT_556AR;
		// 357
	    case 5: return amount * AMMOWEIGHT_357;
	}
	return 0.0f;
}

void CBasePlayer::UpdatePlayerMaxSpeed()
{
	// We only do this for survivors!!!
	if ( pev->team != ZP::TEAM_SURVIVIOR ) return;

	int i;
	int iHowFatAmI = 0;

	// Count our ammo
	iHowFatAmI += GetAmmoWeight( "9mm" );
	iHowFatAmI += GetAmmoWeight( "357" );
	iHowFatAmI += GetAmmoWeight( "buckshot" );
	iHowFatAmI += GetAmmoWeight( "556ar" );

	for ( i = 0; i < MAX_ITEM_TYPES; i++ )
	{
		if ( m_rgpPlayerItems[i] )
			iHowFatAmI += m_rgpPlayerItems[i]->iWeight();
	}

#if !defined( CLIENT_DLL )
	// If we are in panic, ignore our weight
	if ( IsInPanic() )
		iHowFatAmI = 0;
#endif

	// Now check the weapons we got.
	float flNewSpeed = ZP::MaxSpeeds[0] - iHowFatAmI;
	if ( flNewSpeed < 50 )
		flNewSpeed = 50;

	pev->maxspeed = flNewSpeed;
}