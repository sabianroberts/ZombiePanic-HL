// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "zp/info_random_base.h"

class CRandomItemWeapon : public CRandomItemBase
{
public:
	virtual const char *GetRandomClassname() const;
	ItemType GetType() const { return ItemType::TypeWeapon; }
};
LINK_ENTITY_TO_CLASS( info_random_weapon, CRandomItemWeapon );

static std::vector<SpawnList> s_SpawnList = {
	{ "weapon_sig", 2, 0, false },
	{ "weapon_357", 2, 2, false },
	{ "weapon_556ar", 2, 4, false },
	{ "weapon_mp5", 3, 3, false },
	{ "weapon_shotgun", 2, 5, false }
};

const char *CRandomItemWeapon::GetRandomClassname() const
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
	} while ( IsLimited( item ) );

	// How many items did we find?
	int iFound = 0;

	// Now we check how many there are on the map
	CBaseEntity *pFind = UTIL_FindEntityByClassname( nullptr, item.Classname );
	while ( pFind )
	{
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

void ResetRandomWeaponSpawnList()
{
	for ( size_t i = 0; i < s_SpawnList.size(); i++ )
	{
		SpawnList &item = s_SpawnList[i];
		item.Full = false;
	}
}
