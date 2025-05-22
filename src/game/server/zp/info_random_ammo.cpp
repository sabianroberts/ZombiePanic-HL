// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "zp/info_random_base.h"

class CRandomItemAmmo : public CRandomItemBase
{
public:
	virtual const char *GetRandomClassname() const;
	ItemType GetType() const { return ItemType::TypeAmmo; }
	bool WeaponExistInWorld( const char *szAmmoType ) const;
};
LINK_ENTITY_TO_CLASS( info_random_ammo, CRandomItemAmmo );

const char *CRandomItemAmmo::GetRandomClassname() const
{
	int iRetryState = 5;
	const char *szItemToSpawn = nullptr;

tryagain:
	switch ( RANDOM_LONG( 0, 4 ) )
	{
		case 0: szItemToSpawn = "ammo_9mmclip"; break;
		case 1: szItemToSpawn = "ammo_556AR"; break;
		case 2: szItemToSpawn = "ammo_556box"; break;
		case 3: szItemToSpawn = "ammo_buckshot"; break;
		case 4: szItemToSpawn = "ammo_357"; break;
	}

	// Now we check if the weapon in question exist, else pick something else (we do this 5 times)
	if ( !WeaponExistInWorld( szItemToSpawn ) )
	{
		// Try again, but if we hit 0, then simply return with a ammo_9mmclip instead
		// because players spawn with those
		if ( iRetryState > 0 )
			iRetryState--;
		else
			return "ammo_9mmclip";
		goto tryagain;
	}

	return szItemToSpawn;
}

bool CRandomItemAmmo::WeaponExistInWorld( const char *szAmmoType ) const
{
	const char *szEntToCheck = nullptr;
	if ( FStrEq( szAmmoType, "ammo_9mmclip" ) ) szEntToCheck = "weapon_9mmhandgun";
	else if ( FStrEq( szAmmoType, "ammo_556AR" ) || FStrEq( szAmmoType, "ammo_556box" ) ) szEntToCheck = "weapon_556AR";
	else if ( FStrEq( szAmmoType, "ammo_buckshot" ) ) szEntToCheck = "weapon_shotgun";
	else if ( FStrEq( szAmmoType, "ammo_357" ) ) szEntToCheck = "weapon_357";

	if ( !szEntToCheck ) return false;
	// We only need to find one instance of this classname
	CBaseEntity *pFind = UTIL_FindEntityByClassname( nullptr, szEntToCheck );
	return pFind ? true : false;
}
