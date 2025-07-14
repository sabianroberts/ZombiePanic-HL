// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "player.h"
#include "zp/info_random_base.h"

void CRandomItemBase::SpawnItem(void)
{
	const char *szItemToSpawn = GetRandomClassname();
	if ( szItemToSpawn && szItemToSpawn[0] )
	{
		CBaseEntity *pSpawned = CBaseEntity::Create( (char *)szItemToSpawn, pev->origin + Vector( 0, 0, 2 ), pev->angles, nullptr );
		if ( pSpawned )
			pSpawned->SetSpawnedTroughRandomEntity( true );
	}
}

static int s_iCurrentPlayerAmount = 0;
static void CheckCurrentPlayers()
{
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *plr = (CBasePlayer *)UTIL_PlayerByIndex( i );
		if ( plr && plr->IsConnected() )
			s_iCurrentPlayerAmount++;
	}
}

bool CRandomItemBase::IsLimited( SpawnList item ) const
{
	if ( item.PlayersRequired > s_iCurrentPlayerAmount ) return true;
	if ( item.Limit <= 0 ) return false;
	if ( item.Full ) return true;
	return false;
}

extern void ResetRandomWeaponSpawnList();
extern void ResetRandomAmmoSpawnList();

void ZP::SpawnWeaponsFromRandomEntities()
{
	// Check all players first
	CheckCurrentPlayers();

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

	// Reset after use
	ResetRandomWeaponSpawnList();
	ResetRandomAmmoSpawnList();
	s_iCurrentPlayerAmount = 0;
}
