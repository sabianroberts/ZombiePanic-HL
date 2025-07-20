// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "zp/info_random_base.h"

class CRandomItemItem : public CRandomItemBase
{
public:
	const char *GetRandomClassname() const;
	ItemType GetType() const { return ItemType::TypeItem; }
};
LINK_ENTITY_TO_CLASS( info_random_item, CRandomItemItem );

static std::vector<SpawnList> s_SpawnList = {
	{ "item_healthkit", 4, 0, false },
	{ "item_battery", 4, 0, false },
	{ "weapon_satchel", 4, 4, false },
	{ "weapon_handgrenade", 3, 3, false }
};

const char *CRandomItemItem::GetRandomClassname() const
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
	} while ( !IsLimited( item ) );
	if ( !item.Classname ) return nullptr;

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
