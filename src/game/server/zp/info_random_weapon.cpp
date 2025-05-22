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

const char *CRandomItemWeapon::GetRandomClassname() const
{
	switch ( RANDOM_LONG( 0, 3 ) )
	{
		case 0: return "weapon_9mmhandgun";
		case 1: return "weapon_357";
		case 2: return "weapon_556AR";
		case 3: return "weapon_shotgun";
	}
}
