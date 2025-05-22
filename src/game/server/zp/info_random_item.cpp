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

const char *CRandomItemItem::GetRandomClassname() const
{
	switch ( RANDOM_LONG( 0, 3 ) )
	{
		case 0: return "item_healthkit";
		case 1: return "item_battery";
		case 2: return "weapon_satchel";
		case 3: return "weapon_handgrenade";
	}
}
