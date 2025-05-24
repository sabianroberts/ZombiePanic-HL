// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "zp/info_random_base.h"

void CRandomItemBase::SpawnItem(void)
{
	const char *szItemToSpawn = GetRandomClassname();
	if ( szItemToSpawn && szItemToSpawn[0] )
	{
		CBaseEntity *pSpawned = CBaseEntity::Create( (char *)szItemToSpawn, pev->origin, pev->angles, nullptr );
		if ( pSpawned )
			pSpawned->SetSpawnedTroughRandomEntity( true );
	}
}

void ZP::SpawnWeaponsFromRandomEntities()
{
	// First spawn all weapons
	CRandomItemBase *pFind = (CRandomItemBase *)UTIL_FindEntityByClassname( nullptr, "info_random_weapon" );
	while ( pFind )
	{
		pFind->SpawnItem();
		pFind = (CRandomItemBase *)UTIL_FindEntityByClassname( pFind, "info_random_weapon" );
	}

	// Now go trough our ammo.
	pFind = (CRandomItemBase *)UTIL_FindEntityByClassname( nullptr, "info_random_ammo" );
	while ( pFind )
	{
		pFind->SpawnItem();
		pFind = (CRandomItemBase *)UTIL_FindEntityByClassname( pFind, "info_random_ammo" );
	}

	// Now go trough our items.
	pFind = (CRandomItemBase *)UTIL_FindEntityByClassname( nullptr, "info_random_item" );
	while ( pFind )
	{
		pFind->SpawnItem();
		pFind = (CRandomItemBase *)UTIL_FindEntityByClassname( pFind, "info_random_item" );
	}
}
