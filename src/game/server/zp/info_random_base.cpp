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
static std::vector<SpawnList*> s_SpawnList;
static SpawnList *m_DefaultSpawnItem = new SpawnList();

static void OnItemCreated(const char *szClassname)
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
static void SetItemAsFull( const char *szClassname )
{
	for ( size_t i = 0; i < s_SpawnList.size(); i++ )
	{
		SpawnList *item = s_SpawnList[i];
		if ( FStrEq( item->Classname, szClassname ) )
			item->Full = true;
	}
}

void ZP::CheckHowManySpawnedItems( CBasePlayer *pPlayer )
{
	bool bIsCheatsEnabled = CVAR_GET_FLOAT("sv_cheats") >= 1 ? true : false;
	if ( !bIsCheatsEnabled ) return;
	UTIL_PrintConsole( "Items spawned this round:\n", pPlayer );
	for (DebugSpawnList &i : s_SpawnedItems)
		UTIL_PrintConsole( UTIL_VarArgs( "%s [%i]\n", i.Classname.c_str(), i.Amount ), pPlayer );
	UTIL_PrintConsole( "--------------------\n", pPlayer );
}

void CRandomItemBase::SpawnItem(void)
{
	int istr = GetRandomClassname();
	if ( istr == 0 ) return;
	const char *szItemToSpawn = STRING( istr );
	if ( szItemToSpawn && szItemToSpawn[0] )
	{
		edict_t *pent = CREATE_NAMED_ENTITY( istr );
		if ( FNullEnt(pent) ) return;

		VARS(pent)->origin = pev->origin + Vector( 0, 0, 2 );
		VARS(pent)->angles = pev->angles;
		pent->v.spawnflags |= SF_NORESPAWN;
		DispatchSpawn(pent);

		CBaseEntity *pSpawned = (CBaseEntity *)GET_PRIVATE(pent);
		if ( pSpawned )
		{
			pSpawned->SetSpawnedTroughRandomEntity( true );
			OnItemCreated( szItemToSpawn );
		}
	}
}

string_t CRandomItemBase::GetRandomClassname() const
{
	std::vector<SpawnList*> temp = s_SpawnList;
	int idx = RANDOM_LONG( 0, temp.size() - 1 );
	SpawnList *item = temp[ idx ];

	bool bNotValidItem = IsLimited( item );
	while ( bNotValidItem )
	{
		// All of it was invalid?
		// Return default value then.
		if ( temp.size() == 0 )
		{
			item = m_DefaultSpawnItem;
			break;
		}
		else
		{
			idx = RANDOM_LONG( 0, temp.size() - 1 );
			item = temp[ idx ];
			temp.erase( temp.begin() + idx );
		}
		bNotValidItem = IsLimited( item );
	}
	if ( FStrEq( item->Classname, "" ) ) return 0;

	// How many items did we find?
	// We also start at 1, since *this* item needs to be counted for.
	int iFound = 1;

	// Now we check how many there are on the map
	CBaseEntity *pFind = UTIL_FindEntityByClassname( nullptr, item->Classname );
	while ( pFind )
	{
		iFound++;
		pFind = UTIL_FindEntityByClassname( pFind, item->Classname );
	}

	// Check how many we got, if it's equal or above, make Full true
	if ( iFound >= item->Limit )
		SetItemAsFull( item->Classname );

	return ALLOC_STRING( item->Classname );
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

	// TODO: Move this to Angelscript once implemented.
#define ItemSpawner s_SpawnList
	int nAmmoToSpawn[5];
	int nWeaponToSpawn[5];
	int nItemToSpawn[4];
	int nPlayers = s_iCurrentPlayerAmount;
	if ( nPlayers >= 18 )
	{
		nAmmoToSpawn[0] = 3; // 0 - ammo_9mmclip
		nAmmoToSpawn[1] = 8; // 1 - ammo_mp5clip
		nAmmoToSpawn[2] = 4; // 2 - ammo_556AR
		nAmmoToSpawn[3] = 4; // 3 - ammo_buckshot
		nAmmoToSpawn[4] = 4; // 4 - ammo_357

		nWeaponToSpawn[0] = 2; // 0 - weapon_sig
		nWeaponToSpawn[1] = 2; // 1 - weapon_357
		nWeaponToSpawn[2] = 2; // 2 - weapon_556ar
		nWeaponToSpawn[3] = 2; // 3 - weapon_mp5
		nWeaponToSpawn[4] = 2; // 4 - weapon_shotgun

		nItemToSpawn[0] = 3; // 0 - item_healthkit
		nItemToSpawn[1] = 3; // 1 - item_battery
		nItemToSpawn[2] = 3; // 2 - weapon_satchel
		nItemToSpawn[3] = 3; // 3 - weapon_handgrenade
	}
	else if ( nPlayers >= 15 )
	{
		nAmmoToSpawn[0] = 6; // 0 - ammo_9mmclip
		nAmmoToSpawn[1] = 5; // 1 - ammo_mp5clip
		nAmmoToSpawn[2] = 3; // 2 - ammo_556AR
		nAmmoToSpawn[3] = 3; // 3 - ammo_buckshot
		nAmmoToSpawn[4] = 3; // 4 - ammo_357

		nWeaponToSpawn[0] = 2; // 0 - weapon_sig
		nWeaponToSpawn[1] = 2; // 1 - weapon_357
		nWeaponToSpawn[2] = 2; // 2 - weapon_556ar
		nWeaponToSpawn[3] = 2; // 3 - weapon_mp5
		nWeaponToSpawn[4] = 2; // 4 - weapon_shotgun

		nItemToSpawn[0] = 3; // 0 - item_healthkit
		nItemToSpawn[1] = 3; // 1 - item_battery
		nItemToSpawn[2] = 2; // 2 - weapon_satchel
		nItemToSpawn[3] = 3; // 3 - weapon_handgrenade
	}
	else if ( nPlayers >= 12 )
	{
		nAmmoToSpawn[0] = 6; // 0 - ammo_9mmclip
		nAmmoToSpawn[1] = 5; // 1 - ammo_mp5clip
		nAmmoToSpawn[2] = 2; // 2 - ammo_556AR
		nAmmoToSpawn[3] = 2; // 3 - ammo_buckshot
		nAmmoToSpawn[4] = 3; // 4 - ammo_357

		nWeaponToSpawn[0] = 2; // 0 - weapon_sig
		nWeaponToSpawn[1] = 2; // 1 - weapon_357
		nWeaponToSpawn[2] = 1; // 2 - weapon_556ar
		nWeaponToSpawn[3] = 2; // 3 - weapon_mp5
		nWeaponToSpawn[4] = 1; // 4 - weapon_shotgun

		nItemToSpawn[0] = 3; // 0 - item_healthkit
		nItemToSpawn[1] = 2; // 1 - item_battery
		nItemToSpawn[2] = 1; // 2 - weapon_satchel
		nItemToSpawn[3] = 3; // 3 - weapon_handgrenade
	}
	else if ( nPlayers >= 8 )
	{
		nAmmoToSpawn[0] = 6; // 0 - ammo_9mmclip
		nAmmoToSpawn[1] = 3; // 1 - ammo_mp5clip
		nAmmoToSpawn[2] = 0; // 2 - ammo_556AR
		nAmmoToSpawn[3] = 2; // 3 - ammo_buckshot
		nAmmoToSpawn[4] = 3; // 4 - ammo_357

		nWeaponToSpawn[0] = 2; // 0 - weapon_sig
		nWeaponToSpawn[1] = 2; // 1 - weapon_357
		nWeaponToSpawn[2] = 0; // 2 - weapon_556ar
		nWeaponToSpawn[3] = 1; // 3 - weapon_mp5
		nWeaponToSpawn[4] = 1; // 4 - weapon_shotgun

		nItemToSpawn[0] = 2; // 0 - item_healthkit
		nItemToSpawn[1] = 2; // 1 - item_battery
		nItemToSpawn[2] = 1; // 2 - weapon_satchel
		nItemToSpawn[3] = 2; // 3 - weapon_handgrenade
	}
	else if ( nPlayers >= 5 )
	{
		nAmmoToSpawn[0] = 5; // 0 - ammo_9mmclip
		nAmmoToSpawn[1] = 2; // 1 - ammo_mp5clip
		nAmmoToSpawn[2] = 0; // 2 - ammo_556AR
		nAmmoToSpawn[3] = 1; // 3 - ammo_buckshot
		nAmmoToSpawn[4] = 2; // 4 - ammo_357

		nWeaponToSpawn[0] = 2; // 0 - weapon_sig
		nWeaponToSpawn[1] = 1; // 1 - weapon_357
		nWeaponToSpawn[2] = 0; // 2 - weapon_556ar
		nWeaponToSpawn[3] = 1; // 3 - weapon_mp5
		nWeaponToSpawn[4] = 1; // 4 - weapon_shotgun

		nItemToSpawn[0] = 2; // 0 - item_healthkit
		nItemToSpawn[1] = 1; // 1 - item_battery
		nItemToSpawn[2] = 1; // 2 - weapon_satchel
		nItemToSpawn[3] = 2; // 3 - weapon_handgrenade
	}
	else if ( nPlayers >= 3 )
	{
		nAmmoToSpawn[0] = 4; // 0 - ammo_9mmclip
		nAmmoToSpawn[1] = 0; // 1 - ammo_mp5clip
		nAmmoToSpawn[2] = 0; // 2 - ammo_556AR
		nAmmoToSpawn[3] = 0; // 3 - ammo_buckshot
		nAmmoToSpawn[4] = 2; // 4 - ammo_357

		nWeaponToSpawn[0] = 2; // 0 - weapon_sig
		nWeaponToSpawn[1] = 1; // 1 - weapon_357
		nWeaponToSpawn[2] = 0; // 2 - weapon_556ar
		nWeaponToSpawn[3] = 0; // 3 - weapon_mp5
		nWeaponToSpawn[4] = 0; // 4 - weapon_shotgun

		nItemToSpawn[0] = 2; // 0 - item_healthkit
		nItemToSpawn[1] = 1; // 1 - item_battery
		nItemToSpawn[2] = 0; // 2 - weapon_satchel
		nItemToSpawn[3] = 1; // 3 - weapon_handgrenade
	}
	else
	{
		nAmmoToSpawn[0] = 4;	// 0 - ammo_9mmclip
		nAmmoToSpawn[1] = 0;	// 1 - ammo_mp5clip
		nAmmoToSpawn[2] = 0;	// 2 - ammo_556AR
		nAmmoToSpawn[3] = 0;	// 3 - ammo_buckshot
		nAmmoToSpawn[4] = 0;	// 4 - ammo_357

		nWeaponToSpawn[0] = 2;	// 0 - weapon_sig
		nWeaponToSpawn[1] = 0;	// 1 - weapon_357
		nWeaponToSpawn[2] = 0;	// 2 - weapon_556ar
		nWeaponToSpawn[3] = 0;	// 3 - weapon_mp5
		nWeaponToSpawn[4] = 0;	// 4 - weapon_shotgun

		nItemToSpawn[0] = 1;	// 0 - item_healthkit
		nItemToSpawn[1] = 1;	// 1 - item_battery
		nItemToSpawn[2] = 0;	// 2 - weapon_satchel
		nItemToSpawn[3] = 1;	// 3 - weapon_handgrenade
	}

	// Ammo
	ItemSpawner.push_back( new SpawnList( "ammo_9mmclip", nAmmoToSpawn[0], ItemType::TypeAmmo ) );
	ItemSpawner.push_back( new SpawnList( "ammo_mp5clip", nAmmoToSpawn[1], ItemType::TypeAmmo ) );
	ItemSpawner.push_back( new SpawnList( "ammo_556AR", nAmmoToSpawn[2], ItemType::TypeAmmo ) );
	ItemSpawner.push_back( new SpawnList( "ammo_buckshot", nAmmoToSpawn[3], ItemType::TypeAmmo ) );
	ItemSpawner.push_back( new SpawnList( "ammo_357", nAmmoToSpawn[4], ItemType::TypeAmmo ) );

	// Items
	ItemSpawner.push_back( new SpawnList( "item_healthkit", nItemToSpawn[0], ItemType::TypeItem ) );
	ItemSpawner.push_back( new SpawnList( "item_battery", nItemToSpawn[1], ItemType::TypeItem ) );
	ItemSpawner.push_back( new SpawnList( "weapon_satchel", nItemToSpawn[2], ItemType::TypeItem ) );
	ItemSpawner.push_back( new SpawnList( "weapon_handgrenade", nItemToSpawn[3], ItemType::TypeItem ) );

	// Weapons
	ItemSpawner.push_back( new SpawnList( "weapon_sig", nWeaponToSpawn[0], ItemType::TypeWeapon ) );
	ItemSpawner.push_back( new SpawnList( "weapon_357", nWeaponToSpawn[1], ItemType::TypeWeapon ) );
	ItemSpawner.push_back( new SpawnList( "weapon_556ar", nWeaponToSpawn[2], ItemType::TypeWeapon ) );
	ItemSpawner.push_back( new SpawnList( "weapon_mp5", nWeaponToSpawn[3], ItemType::TypeWeapon ) );
	ItemSpawner.push_back( new SpawnList( "weapon_shotgun", nWeaponToSpawn[4], ItemType::TypeWeapon ) );
}

bool CRandomItemBase::IsLimited( SpawnList *item ) const
{
	// Not the correct type, return true.
	if ( item->Type != GetType() ) return true;
	// if zero, we cannot spawn this.
	if ( item->Limit == 0 ) return true;
	// Hit the limit?
	if ( item->Full ) return true;
	return false;
}

void ZP::SpawnWeaponsFromRandomEntities()
{
	// Clear it
	s_SpawnedItems.clear();
	for ( auto p : s_SpawnList )
		delete p;
	s_SpawnList.clear();

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
	for ( size_t i = 0; i < s_SpawnList.size(); i++ )
	{
		SpawnList *item = s_SpawnList[i];
		item->Full = false;
	}
	s_iCurrentPlayerAmount = 0;
}
