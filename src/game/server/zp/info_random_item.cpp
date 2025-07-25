// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "zp/info_random_base.h"

class CRandomItemItem : public CRandomItemBase
{
public:
	ItemType GetType() const { return ItemType::TypeItem; }
};
LINK_ENTITY_TO_CLASS( info_random_item, CRandomItemItem );