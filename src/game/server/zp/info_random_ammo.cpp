// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "zp/info_random_base.h"

class CRandomItemAmmo : public CRandomItemBase
{
public:
	virtual const char *GetRandomClassname() const;
	ItemType GetType() const { return ItemType::TypeAmmo; }
	bool WeaponExistInWorld( SpawnList item ) const;
};
LINK_ENTITY_TO_CLASS( info_random_ammo, CRandomItemAmmo );

static std::vector<SpawnList> s_SpawnList = {
	{ "ammo_9mmclip", 18, 0, false },
	{ "ammo_mp5clip", 10, 0, false },
	{ "ammo_556AR", 5, 0, false },
	{ "ammo_buckshot", 5, 0, false },
	{ "ammo_357", 7, 0, false }
};

const char *CRandomItemAmmo::GetRandomClassname() const
{
	std::vector<SpawnList> temp = s_SpawnList;
	int idx = RANDOM_LONG( 0, temp.size() - 1 );
	SpawnList item = temp[ idx ];
	do
	{
		// All of it was invalid?
		// Return default value then.
		if ( temp.size() == 0 )
			item = SpawnList();
		else
		{
			idx = RANDOM_LONG( 0, temp.size() - 1 );
			item = temp[ idx ];
			temp.erase( temp.begin() + idx );
		}
	} while ( !WeaponExistInWorld( item ) );
	if ( !item.Classname ) return nullptr;

	// How many items did we find?
	int iFound = 0;

	// Now we check how many there are on the map
	CBaseEntity *pFind = UTIL_FindEntityByClassname( nullptr, item.Classname );
	while ( pFind )
	{
		// Ignore players (if they spawned with X item)
		if ( !pFind->pev->owner )
			iFound++;
		pFind = UTIL_FindEntityByClassname( pFind, item.Classname );
	}

	// Check how many we got, if it's equal or above, make Full true
	if ( iFound >= item.Limit )
	{
		SpawnList &item = s_SpawnList[idx];
		item.Full = true;
	}

	return item.Classname;
}

bool CRandomItemAmmo::WeaponExistInWorld( SpawnList item ) const
{
	if ( !item.Classname ) return true;
	const char *szEntToCheck = nullptr;
	if ( FStrEq( item.Classname, "ammo_9mmclip" ) ) szEntToCheck = "weapon_sig";
	else if ( FStrEq( item.Classname, "ammo_mp5clip" ) ) szEntToCheck = "weapon_mp5";
	else if ( FStrEq( item.Classname, "ammo_556AR" ) ) szEntToCheck = "weapon_556ar";
	else if ( FStrEq( item.Classname, "ammo_buckshot" ) ) szEntToCheck = "weapon_shotgun";
	else if ( FStrEq( item.Classname, "ammo_357" ) ) szEntToCheck = "weapon_357";

	if ( !szEntToCheck ) return false;
	// We only need to find one instance of this classname
	CBaseEntity *pFind = UTIL_FindEntityByClassname( nullptr, szEntToCheck );
	return pFind ? !IsLimited( item ) : false;
}

void ResetRandomAmmoSpawnList()
{
	for ( size_t i = 0; i < s_SpawnList.size(); i++ )
	{
		SpawnList &item = s_SpawnList[i];
		item.Full = false;
	}
}
