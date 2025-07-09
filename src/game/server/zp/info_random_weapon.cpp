// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "zp/info_random_base.h"

struct SpawnList
{
	const char *Weapon = nullptr;
	int iLimit = 0;
	bool Full = false;
};

class CRandomItemWeapon : public CRandomItemBase
{
public:
	virtual const char *GetRandomClassname() const;
	bool IsWeaponLimited( SpawnList item ) const;
	ItemType GetType() const { return ItemType::TypeWeapon; }
};
LINK_ENTITY_TO_CLASS( info_random_weapon, CRandomItemWeapon );

static std::vector<SpawnList> s_WeaponSpawnList = {
	{ "weapon_sig", 3, false },
	{ "weapon_357", 3, false },
	{ "weapon_556ar", 2, false },
	{ "weapon_mp5", 3, false },
	{ "weapon_shotgun", 3, false }
};

const char *CRandomItemWeapon::GetRandomClassname() const
{
	std::vector<SpawnList> temp = s_WeaponSpawnList;
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
	} while ( IsWeaponLimited( item ) );

	// How many items did we find?
	int iFound = 0;

	// Now we check how many there are on the map
	CBaseEntity *pFind = UTIL_FindEntityByClassname( nullptr, item.Weapon );
	while ( pFind )
	{
		// Ignore players (if they spawned with X item)
		if ( !pFind->pev->owner )
			iFound++;
		pFind = UTIL_FindEntityByClassname( pFind, item.Weapon );
	}

	// Check how many we got, if it's equal or above, make Full true
	if ( iFound >= item.iLimit )
	{
		SpawnList &item = s_WeaponSpawnList[idx];
		item.Full = true;
	}

	return item.Weapon;
}

bool CRandomItemWeapon::IsWeaponLimited( SpawnList item ) const
{
	if ( item.iLimit <= 0 ) return false;
	if ( item.Full ) return true;
	return false;
}

void ResetRandomWeaponSpawnList()
{
	for ( size_t i = 0; i < s_WeaponSpawnList.size(); i++ )
	{
		SpawnList &item = s_WeaponSpawnList[i];
		item.Full = false;
	}
}