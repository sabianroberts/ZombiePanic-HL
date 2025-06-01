// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "cbase.h"

#if defined(CLIENT_DLL)
#include "strtools.h"
#endif

#include <tier2/tier2.h>
#include "FileSystem.h"
#include <KeyValues.h>

#include "weapons.h"
#include "player.h"

#include "zp_shared.h"

#define WEAPON_SCRIPT_PATH "scripts/weapon_"
#define WEAPON_SCRIPT_FILE ".txt"
static std::vector<WeaponData> sWeaponDataList;
static AmmoData sAmmoDataList[] = {
	/* { ZPAmmoTypes AmmoType, const char *AmmoName, int MaxCarry, float WeightPerBullet } */
	{ AMMO_PISTOL, "9mm", 150, 0.21f },
	{ AMMO_MAGNUM, "357", 30, 0.65f },
	{ AMMO_SHOTGUN, "buckshot", 80, 1.25f },
	{ AMMO_RIFLE, "556ar", 240, 0.21f },

	{ AMMO_GRENADE, "Hand Grenade", 3, 0.0f },
	{ AMMO_SATCHEL, "Satchel Charge", 5, 0.0f },

	// MUST BE LAST, DO NOT CHANGE THIS.
	// This is used by Crowbar and Swipe (or any other weapon that has no ammo)
	{ AMMO_NONE, "", -1, 0.0f }
};

WeaponData CreateWeaponSlotData( ZPWeaponID WeaponID )
{
	const char *szWeaponScriptFile = nullptr;
	// TODO: Change this to use pev->classname instead later?
	// example output should be: "scripts/weapon_crowbar.txt"
	switch ( WeaponID )
	{
		case WEAPON_CROWBAR: szWeaponScriptFile = WEAPON_SCRIPT_PATH "crowbar" WEAPON_SCRIPT_FILE; break;
		case WEAPON_SWIPE: szWeaponScriptFile = WEAPON_SCRIPT_PATH "swipe" WEAPON_SCRIPT_FILE; break;
		case WEAPON_GLOCK: szWeaponScriptFile = WEAPON_SCRIPT_PATH "beretta" WEAPON_SCRIPT_FILE; break;
		case WEAPON_PYTHON: szWeaponScriptFile = WEAPON_SCRIPT_PATH "357" WEAPON_SCRIPT_FILE; break;
		case WEAPON_MP5: szWeaponScriptFile = WEAPON_SCRIPT_PATH "mp5" WEAPON_SCRIPT_FILE; break;
		case WEAPON_556AR: szWeaponScriptFile = WEAPON_SCRIPT_PATH "556ar" WEAPON_SCRIPT_FILE; break;
		case WEAPON_SHOTGUN: szWeaponScriptFile = WEAPON_SCRIPT_PATH "shotgun" WEAPON_SCRIPT_FILE; break;
		case WEAPON_HANDGRENADE: szWeaponScriptFile = WEAPON_SCRIPT_PATH "grenade" WEAPON_SCRIPT_FILE; break;
		case WEAPON_SATCHEL: szWeaponScriptFile = WEAPON_SCRIPT_PATH "satchel" WEAPON_SCRIPT_FILE; break;
		default: szWeaponScriptFile = WEAPON_SCRIPT_PATH "example" WEAPON_SCRIPT_FILE; break;
	}
	if ( !szWeaponScriptFile ) return WeaponData();
#if defined( CLIENT_DLL )
	UTIL_LogPrintf("[CLIENT] Loading weapon script \"%s\" [i]\n", szWeaponScriptFile, WeaponID );
#else
	UTIL_LogPrintf("[SERVER] Loading weapon script \"%s\" [i]\n", szWeaponScriptFile, WeaponID );
#endif
	KeyValues *pWeaponScript = new KeyValues( "WeaponInfo" );
	if ( !pWeaponScript->LoadFromFile( g_pFullFileSystem, szWeaponScriptFile ) )
	{
		pWeaponScript->deleteThis();
		return WeaponData();
	}

	WeaponData slot;
	slot.WeaponID = WeaponID;
	slot.Ammo1[0] = 0;
	slot.Ammo2[0] = 0;

	// Default
	slot.FireRate[0] = 0.1f;
	slot.FireRate[1] = 0.1f;
	slot.WeaponSpread[0] = 0.1f;
	slot.WeaponSpread[1] = 0.1f;

	slot.Slot = pWeaponScript->GetInt( "Slot", 0 );
	slot.Position = pWeaponScript->GetInt( "Position", 0 );

	const char *szAmmoType = pWeaponScript->GetString( "AmmoType1", NULL );
	if ( szAmmoType && szAmmoType[0] )
	{
		AmmoData ammo = GetAmmoByName( szAmmoType );
		if ( ammo.MaxCarry != -1 )
			UTIL_strcpy( slot.Ammo1, szAmmoType );
	}

	szAmmoType = pWeaponScript->GetString( "AmmoType2", NULL );
	if ( szAmmoType && szAmmoType[0] )
	{
		AmmoData ammo = GetAmmoByName( szAmmoType );
		if ( ammo.MaxCarry != -1 )
			UTIL_strcpy(slot.Ammo2, szAmmoType);
	}

	slot.DefaultAmmo = pWeaponScript->GetInt( "DefaultGive", 0 );
	slot.MaxClip = pWeaponScript->GetInt( "MaxClip", 0 );
	slot.Flags = 0;

	// Go trough our flags
	KeyValues *pWeaponFlags = pWeaponScript->FindKey( "Flags" );
	if ( pWeaponFlags )
	{
		if ( pWeaponFlags->GetBool( "SELECTONEMPTY" ) ) slot.Flags |= ITEM_FLAG_SELECTONEMPTY;
		if ( pWeaponFlags->GetBool( "NOAUTORELOAD" ) ) slot.Flags |= ITEM_FLAG_NOAUTORELOAD;
		if ( pWeaponFlags->GetBool( "NOAUTOSWITCHEMPTY" ) ) slot.Flags |= ITEM_FLAG_NOAUTOSWITCHEMPTY;
		if ( pWeaponFlags->GetBool( "LIMITINWORLD" ) ) slot.Flags |= ITEM_FLAG_LIMITINWORLD;
		if ( pWeaponFlags->GetBool( "EXHAUSTIBLE" ) ) slot.Flags |= ITEM_FLAG_EXHAUSTIBLE;
		if ( pWeaponFlags->GetBool( "NOAUTOSWITCHTO" ) ) slot.Flags |= ITEM_FLAG_NOAUTOSWITCHTO;
	}

	slot.Weight = pWeaponScript->GetFloat( "Weight", 0 );

	// Primary Attack stuff
	KeyValues *pPrimaryAttack = pWeaponScript->FindKey( "Primary" );
	if ( pPrimaryAttack )
	{
		slot.FireRate[0] = pPrimaryAttack->GetFloat( "FireRate", 0.1f );
		slot.WeaponSpread[0] = pPrimaryAttack->GetFloat( "Spread", 0.1f );
	}

	// Secondary Attack stuff
	KeyValues *pSecondaryAttack = pWeaponScript->FindKey( "Secondary" );
	if ( pSecondaryAttack )
	{
		slot.FireRate[1] = pSecondaryAttack->GetFloat( "FireRate", 0.1f );
		slot.WeaponSpread[1] = pSecondaryAttack->GetFloat( "Spread", 0.1f );
	}

	pWeaponScript->deleteThis();

#if defined( CLIENT_DLL )
	UTIL_LogPrintf("[CLIENT] Loaded weapon script \"%s\" [%i]\n", szWeaponScriptFile, slot.WeaponID );
#else
	UTIL_LogPrintf("[SERVER] Loaded weapon script \"%s\" [%i]\n", szWeaponScriptFile, slot.WeaponID );
#endif

	sWeaponDataList.push_back( slot );
	return slot;
}

AmmoData GetAmmoByName( const char *szAmmoType )
{
	if ( szAmmoType && szAmmoType[0] )
	{
		for ( int i = 0; i < ARRAYSIZE( sAmmoDataList ); i++ )
		{
			AmmoData ammo = sAmmoDataList[i];
			if ( !ammo.AmmoName ) continue;
			if ( FStrEq( ammo.AmmoName, szAmmoType ) )
				return ammo;
		}
	}
	// If we find nothing, grab the last item in the list.
	return sAmmoDataList[ ARRAYSIZE( sAmmoDataList ) - 1 ];
}

AmmoData GetAmmoByTableIndex(int id)
{
	int iFind = (int)clamp( id, 0, ARRAYSIZE( sAmmoDataList ) - 1 );
	return sAmmoDataList[ iFind ];
}

AmmoData GetAmmoByAmmoID( int id )
{
	for ( int i = 0; i < ARRAYSIZE( sAmmoDataList ); i++ )
	{
		AmmoData ammo = sAmmoDataList[i];
		if ( ammo.AmmoType == id )
			return ammo;
	}
	// If we find nothing, grab the last item in the list.
	return sAmmoDataList[ ARRAYSIZE( sAmmoDataList ) - 1 ];
}

AmmoData GetAmmoByWeaponID(ZPWeaponID id, bool bPrimary)
{
	WeaponData slot = GetWeaponSlotInfo( id );
	return GetAmmoByName( bPrimary ? slot.Ammo1 : slot.Ammo2 );
}

WeaponData GetWeaponSlotInfo( ZPWeaponID WeaponID )
{
	for ( int i = 0; i < sWeaponDataList.size(); i++ )
	{
		WeaponData slot = sWeaponDataList[i];
		if ( slot.WeaponID == WeaponID )
			return slot;
	}
	return CreateWeaponSlotData( WeaponID );
}

#ifdef SERVER_DLL
struct StaticSpawn
{
	char Classname[32];
	int SpawnFlags;
	Vector Origin;
	Vector Angle;
};
static std::vector<StaticSpawn> sStaticSpawnList;
void ZP::ClearStaticSpawnList() { sStaticSpawnList.clear(); }

void ZP::AddToStaticSpawnList( string_t classname, int spawnflags, float flOrigin[3], float flAngle[3] )
{
	StaticSpawn item;
	UTIL_strcpy( item.Classname, STRING( classname ) );
	item.SpawnFlags = spawnflags;
	item.Origin.x = flOrigin[0];
	item.Origin.y = flOrigin[1];
	item.Origin.z = flOrigin[2];
	item.Angle.x = flAngle[0];
	item.Angle.y = flAngle[1];
	item.Angle.z = flAngle[2];

	sStaticSpawnList.push_back( item );
}

void ZP::SpawnStaticSpawns()
{
	for ( int i = 0; i < sStaticSpawnList.size(); i++ )
	{
		StaticSpawn slot = sStaticSpawnList[i];
		CBaseEntity::Create( (char *)slot.Classname, slot.Origin + Vector( 0, 0, 2 ), slot.Angle, nullptr );
	}
}
#endif

ZP::GameModeType_e ZP::IsValidGameModeMap(const char *szLevel)
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
	AmmoData ammo = GetAmmoByName( szAmmo );
	int amount = m_rgAmmo[ ammo.AmmoType ];
	return amount * ammo.WeightPerBullet;
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