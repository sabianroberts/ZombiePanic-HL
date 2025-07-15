// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "player.h"
#include "zp/info_random_base.h"

struct DebugSpawnList
{
	std::string Classname;
	int Amount;
};
static std::vector<DebugSpawnList> s_SpawnedItems;
static void OnItemCreated( const char *szClassname )
{
	for ( size_t i = 0; i < s_SpawnedItems.size(); i++ )
	{
		DebugSpawnList &item = s_SpawnedItems[i];
		if ( FStrEq( item.Classname.c_str(), szClassname ) )
		{
			item.Amount++;
			return;
		}
	}
	DebugSpawnList item;
	item.Amount = 1;
	item.Classname = szClassname;
	s_SpawnedItems.push_back( item );
}

void ZP::CheckHowManySpawnedItems( CBasePlayer *pPlayer )
{
	bool bIsCheatsEnabled = CVAR_GET_FLOAT("sv_cheats") >= 1 ? true : false;
	if ( !bIsCheatsEnabled ) return;
	UTIL_PrintConsole( "Items Spawned this round:\n", pPlayer );
	for (DebugSpawnList &i : s_SpawnedItems)
		UTIL_PrintConsole( UTIL_VarArgs( "%s [%i]\n", i.Classname.c_str(), i.Amount ), pPlayer );
	UTIL_PrintConsole( "--------------------\n", pPlayer );
}

void CRandomItemBase::SpawnItem(void)
{
	const char *szItemToSpawn = GetRandomClassname();
	if ( szItemToSpawn && szItemToSpawn[0] )
	{
		CBaseEntity *pSpawned = CBaseEntity::Create( (char *)szItemToSpawn, pev->origin + Vector( 0, 0, 2 ), pev->angles, nullptr );
		if ( pSpawned )
		{
			pSpawned->SetSpawnedTroughRandomEntity( true );
			OnItemCreated( szItemToSpawn );
		}
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
	// Clear it
	s_SpawnedItems.clear();

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
