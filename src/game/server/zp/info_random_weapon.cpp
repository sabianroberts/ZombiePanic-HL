// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "zp/info_random_base.h"

class CRandomItemWeapon : public CRandomItemBase
{
public:
	ItemType GetType() const { return ItemType::TypeWeapon; }
};
LINK_ENTITY_TO_CLASS( info_random_weapon, CRandomItemWeapon );
