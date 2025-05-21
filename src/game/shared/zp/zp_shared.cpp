
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
	switch ( ammoindex )
	{
		// Buckshot
		case 0: return amount * 1.25f;
		// 9mm
	    case 1: return amount * 0.21f;
		// 556AR
	    case 2: return amount * 0.21f;
		// 357
	    case 3: return amount * 0.65f;
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

	// Now check the weapons we got.
	float flNewSpeed = ZP::MaxSpeeds[0] - iHowFatAmI;
	if ( flNewSpeed < 50 )
		flNewSpeed = 50;

	pev->maxspeed = flNewSpeed;
}