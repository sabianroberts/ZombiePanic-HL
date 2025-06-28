/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef WEAPONS_H
#define WEAPONS_H

#include "effects.h"
#include "zp/zp_shared.h"

class CBasePlayer;
extern int gmsgAmmoPickup;
extern int gmsgWeapPickup;

void DeactivateSatchels(CBasePlayer *pOwner);

// Contact Grenade / Timed grenade / Satchel Charge
class CGrenade : public CBaseMonster
{
public:
	void Spawn(void);

	typedef enum
	{
		SATCHEL_DETONATE = 0,
		SATCHEL_RELEASE
	} SATCHELCODE;

	static CGrenade *ShootTimed(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time);
	static CGrenade *ShootContact(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	static CGrenade *ShootSatchelCharge(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	static void UseSatchelCharges(entvars_t *pevOwner, SATCHELCODE code);

	void Explode(Vector vecSrc, Vector vecAim);
	virtual void Explode(TraceResult *pTrace, int bitsDamageType);
	void EXPORT Smoke(void);

	void EXPORT BounceTouch(CBaseEntity *pOther);
	void EXPORT SlideTouch(CBaseEntity *pOther);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT DangerSoundThink(void);
	void EXPORT PreDetonate(void);
	void EXPORT Detonate(void);
	void EXPORT DetonateUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT TumbleThink(void);

	virtual void BounceSound(void);
	virtual int BloodColor(void) { return DONT_BLEED; }
	virtual void Killed(entvars_t *pevAttacker, int iGib);

	BOOL m_fRegisteredSound; // whether or not this grenade has issued its DANGER sound to the world sound list yet.
};

// constant items
#define ITEM_HEALTHKIT 1
#define ITEM_ANTIDOTE  2
#define ITEM_SECURITY  3
#define ITEM_BATTERY   4

#define WEAPON_ALLWEAPONS (~(1 << WEAPON_SUIT))

#define WEAPON_SUIT 31 // ?????

#define MAX_WEAPONS 32

#define MAX_NORMAL_BATTERY 100

// weapon clip/carry ammo capacities
#define URANIUM_MAX_CARRY      100
#define _9MM_MAX_CARRY         250
#define _556AR_MAX_CARRY       250
#define _357_MAX_CARRY         36
#define BUCKSHOT_MAX_CARRY     125
#define BOLT_MAX_CARRY         50
#define ROCKET_MAX_CARRY       5
#define HANDGRENADE_MAX_CARRY  10
#define SATCHEL_MAX_CARRY      5
#define TRIPMINE_MAX_CARRY     5
#define SNARK_MAX_CARRY        15
#define HORNET_MAX_CARRY       8
#define M203_GRENADE_MAX_CARRY 10

// the maximum amount of ammo each weapon's clip can hold
#define WEAPON_NOCLIP -1

//#define CROWBAR_MAX_CLIP		WEAPON_NOCLIP
#define GLOCK_MAX_CLIP       17
#define PYTHON_MAX_CLIP      6
#define MP5_MAX_CLIP         50
#define MP5_DEFAULT_AMMO     25
#define AR556_MAX_CLIP         50
#define AR556_DEFAULT_AMMO     25
#define SHOTGUN_MAX_CLIP     8
#define CROSSBOW_MAX_CLIP    5
#define RPG_MAX_CLIP         1
#define GAUSS_MAX_CLIP       WEAPON_NOCLIP
#define EGON_MAX_CLIP        WEAPON_NOCLIP
#define HORNETGUN_MAX_CLIP   WEAPON_NOCLIP
#define HANDGRENADE_MAX_CLIP WEAPON_NOCLIP
#define SATCHEL_MAX_CLIP     WEAPON_NOCLIP
#define TRIPMINE_MAX_CLIP    WEAPON_NOCLIP
#define SNARK_MAX_CLIP       WEAPON_NOCLIP

// The amount of ammo given to a player by an ammo item.
#define AMMO_URANIUMBOX_GIVE   20
#define AMMO_GLOCKCLIP_GIVE    GLOCK_MAX_CLIP
#define AMMO_357BOX_GIVE       PYTHON_MAX_CLIP
#define AMMO_MP5CLIP_GIVE      MP5_MAX_CLIP
#define AMMO_AR556CLIP_GIVE      AR556_MAX_CLIP
#define AMMO_CHAINBOX_GIVE     200
#define AMMO_AR556BOX_GIVE     100
#define AMMO_M203BOX_GIVE      2
#define AMMO_BUCKSHOTBOX_GIVE  12
#define AMMO_CROSSBOWCLIP_GIVE CROSSBOW_MAX_CLIP
#define AMMO_RPGCLIP_GIVE      RPG_MAX_CLIP
#define AMMO_URANIUMBOX_GIVE   20
#define AMMO_SNARKBOX_GIVE     5

// bullet types
typedef enum
{
	BULLET_NONE = 0,
	BULLET_PLAYER_9MM, // glock
	BULLET_PLAYER_MP5, // mp5
	BULLET_PLAYER_M16, // m16
	BULLET_PLAYER_357, // python
	BULLET_PLAYER_BUCKSHOT, // shotgun
	BULLET_PLAYER_CROWBAR, // crowbar swipe

	BULLET_MONSTER_9MM,
	BULLET_MONSTER_MP5,
	BULLET_MONSTER_12MM,
} Bullet;

#define ITEM_FLAG_SELECTONEMPTY     1
#define ITEM_FLAG_NOAUTORELOAD      2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY 4
#define ITEM_FLAG_LIMITINWORLD      8
#define ITEM_FLAG_EXHAUSTIBLE       16 // A player can totally exhaust their ammo supply and lose this weapon
#define ITEM_FLAG_NOAUTOSWITCHTO    32

#define WEAPON_IS_ONTARGET 0x40

typedef struct
{
	int iSlot;
	int iPosition;
	char pszAmmo1[16]; // ammo 1 type
	char pszAmmo2[16]; // ammo 2 type
	const char *pszName;
	int iMaxClip;
	int iId;
	int iFlags;
	int iWeight;
	float flWeaponSpread[2]; // 0 - Primary, 1 - Secondary
	float flFireRate[2]; // 0 - Primary, 1 - Secondary
} ItemInfo;

// Items that the player has in their inventory that they can use
class CBasePlayerItem : public CBaseAnimating
{
public:
	virtual void SetObjectCollisionBox(void);

	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	void Spawn();
	void Restart();
	virtual int ObjectCaps(void) { return CBaseEntity::ObjectCaps() | FCAP_MUST_RESET; }

	static TYPEDESCRIPTION m_SaveData[];

	virtual int AddToPlayer(CBasePlayer *pPlayer); // return TRUE if the item you want the item added to the player inventory
	virtual int AddDuplicate(CBasePlayerItem *pItem) { return FALSE; } // return TRUE if you want your duplicate removed from world
	void EXPORT DestroyItem(void);
	void EXPORT DefaultTouch(CBaseEntity *pOther); // default weapon touch
	void EXPORT FallThink(void); // when an item is first spawned, this think is run to determine when the object has hit the ground.
	void EXPORT Materialize(void); // make a weapon visible and tangible
	void EXPORT AttemptToMaterialize(void); // the weapon desires to become visible and tangible, if the game rules allow for it
	CBaseEntity *Respawn(void); // copy a weapon
	void FallInit(void);
	void CheckRespawn(void);
	virtual int GetItemInfo(ItemInfo *p) { return 0; }; // returns 0 if struct not filled out
	virtual BOOL CanDeploy(void) { return TRUE; };
	virtual BOOL Deploy() // returns is deploy was successful
	{
		return TRUE;
	};

	virtual BOOL CanHolster(void) { return TRUE; }; // can this weapon be put away right now?
	virtual void Holster(int skiplocal = 0);
	virtual void UpdateItemInfo(void) { return; };

	virtual void ItemPreFrame(void) { return; } // called each frame by the player PreThink
	virtual void ItemPostFrame(void) { return; } // called each frame by the player PostThink

	virtual void Drop(void);
	virtual void Kill(void);
	virtual void AttachToPlayer(CBasePlayer *pPlayer);

	virtual int PrimaryAmmoIndex() { return -1; };
	virtual int SecondaryAmmoIndex() { return -1; };

	virtual int UpdateClientData(CBasePlayer *pPlayer) { return 0; }

	virtual void DisallowPickupFor( float flDisallow ) { m_flDisallowPickup = gpGlobals->time + flDisallow; }

	virtual CBasePlayerItem *GetWeaponPtr(void) { return NULL; };

	static EXPORT ItemInfo ItemInfoArray[MAX_WEAPONS];

	CBasePlayer *m_pPlayer;
	CBasePlayerItem *m_pNext;

	// Weapon ID
	virtual ZPWeaponID GetWeaponID() = 0;

	virtual int iItemSlot(void) { return 0; } // return 0 to MAX_ITEMS_SLOTS, used in hud

	// Client and server should read the same script file
	const WeaponData GetData()
	{
		WeaponData slot = GetWeaponSlotInfo( GetWeaponID() );
		return slot;
	}

	int iItemPosition(void) { return GetData().Position; }
	bool HasValidAmmoType( bool bPrimary )
	{
		AmmoData data = GetAmmoByName( bPrimary ? GetData().Ammo1 : GetData().Ammo2 );
		return ( data.AmmoType != ZPAmmoTypes::AMMO_NONE ) ? true : false;
	}
	int iMaxAmmo1(void) { return GetAmmoByName( GetData().Ammo1 ).MaxCarry; }
	int iMaxAmmo2(void) { return GetAmmoByName( GetData().Ammo2 ).MaxCarry; }
	const char *pszName(void) { return ItemInfoArray[GetWeaponID()].pszName; }
	int iMaxClip(void) { return GetData().MaxClip; }
	int iWeight(void) { return GetData().Weight; }
	int iFlags(void) { return GetData().Flags; }

	float PrimaryFireRate() { return GetData().FireRate[0]; }
	float SecondaryFireRate() { return GetData().FireRate[1]; }
	float PrimaryWeaponSpread() { return GetData().WeaponSpread[0]; }
	float SecondaryWeaponSpread() { return GetData().WeaponSpread[1]; }

	const Vector GetSpreadVector( float flSpread ) { return Vector(flSpread, flSpread, flSpread); }

	float		m_flDisallowPickup = -1;
	Vector		m_SpawnLoc;
	Vector		m_SpawnAngle;
};

// inventory items that
class CBasePlayerWeapon : public CBasePlayerItem
{
public:
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];

	// generic weapon versions of CBasePlayerItem calls
	virtual int AddToPlayer(CBasePlayer *pPlayer);
	virtual int AddDuplicate(CBasePlayerItem *pItem);

	virtual int ExtractAmmo(CBasePlayerWeapon *pWeapon); //{ return TRUE; };			// Return TRUE if you can add ammo to yourself when picked up
	virtual int ExtractClipAmmo(CBasePlayerWeapon *pWeapon); // { return TRUE; };			// Return TRUE if you can add ammo to yourself when picked up

	virtual int AddWeapon(void)
	{
		ExtractAmmo(this);
		return TRUE;
	}; // Return TRUE if you want to add yourself to the player

	int GetItemInfo(ItemInfo *p)
	{
		WeaponData slot = GetWeaponSlotInfo( GetWeaponID() );
		p->pszName = STRING(pev->classname);
		strcpy( p->pszAmmo1, slot.Ammo1 );
		strcpy( p->pszAmmo2, slot.Ammo2 );
		p->iMaxClip = slot.MaxClip;
		p->iSlot = slot.Slot;
		p->iPosition = slot.Position;
		p->iFlags = slot.Flags;
		p->iWeight = slot.Weight;
		p->iId = slot.WeaponID;
		p->flFireRate[0] = slot.FireRate[0];
		p->flFireRate[1] = slot.FireRate[1];
		p->flWeaponSpread[0] = slot.WeaponSpread[0];
		p->flWeaponSpread[1] = slot.WeaponSpread[1];
		return 1;
	}

	// generic "shared" ammo handlers
	BOOL AddPrimaryAmmo(int iCount, char *szName, int iMaxClip, int iMaxCarry);
	BOOL AddSecondaryAmmo(int iCount, char *szName, int iMaxCarry);

	virtual void UpdateItemInfo(void) {}; // updates HUD state

	int m_iPlayEmptySound;
	int m_fFireOnEmpty; // True when the gun is empty and the player is still holding down the
	    // attack key(s)
	virtual BOOL PlayEmptySound(void);
	virtual void ResetEmptySound(void);

	virtual void SendWeaponAnim(int iAnim, int skiplocal = 1, int body = 0); // skiplocal is 1 if client is predicting weapon animations

	virtual BOOL CanDeploy(void);
	virtual BOOL IsUseable(void);
	BOOL DefaultDeploy(char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt, int skiplocal = 0, int body = 0);
	int DefaultReload(int iAnim, float fDelay, int body = 0);

	virtual void ItemPostFrame(void); // called each frame by the player PostThink
	// called by CBasePlayerWeapons ItemPostFrame()
	virtual void PrimaryAttack(void) { return; } // do "+ATTACK"
	virtual void SecondaryAttack(void) { return; } // do "+ATTACK2"
	virtual void Reload(void) { return; } // do "+RELOAD"
	virtual void WeaponIdle(void) { return; } // called when no buttons pressed
	virtual int UpdateClientData(CBasePlayer *pPlayer); // sends hud info to client dll, if things have changed
	virtual void RetireWeapon(void);
	virtual BOOL ShouldWeaponIdle(void) { return FALSE; };
	virtual void Holster(int skiplocal = 0);
	virtual BOOL UseDecrement(void) { return FALSE; };

	virtual bool IsAutomaticWeapon() const { return true; }

	int PrimaryAmmoIndex();
	int SecondaryAmmoIndex();

	void PrintState(void);

	virtual CBasePlayerItem *GetWeaponPtr(void) { return (CBasePlayerItem *)this; };

	float m_flPumpTime;
	int m_fInSpecialReload; // Are we in the middle of a reload for the shotguns
	float m_flNextPrimaryAttack; // soonest time ItemPostFrame will call PrimaryAttack
	float m_flNextSecondaryAttack; // soonest time ItemPostFrame will call SecondaryAttack
	float m_flTimeWeaponIdle; // soonest time ItemPostFrame will call WeaponIdle
	int m_iPrimaryAmmoType; // "primary" ammo index into players m_rgAmmo[]
	int m_iSecondaryAmmoType; // "secondary" ammo index into players m_rgAmmo[]
	int m_iClip; // number of shots left in the primary weapon clip, -1 it not used
	int m_iClientClip; // the last version of m_iClip sent to hud dll
	int m_iClientWeaponState; // the last version of the weapon state sent to hud dll (is current weapon, is on target)
	int m_fInReload; // Are we in the middle of a reload;

	int m_iDefaultAmmo; // how much ammo you get when you pick up this weapon as placed by a level designer.

protected:
	void SendWeaponPickup(CBasePlayer *pPlayer);
};

class CBasePlayerAmmo : public CBaseEntity
{
public:
	virtual void EXPORT Spawn(void);
	void EXPORT DefaultTouch(CBaseEntity *pOther); // default weapon touch
	bool GiveAmmoToPlayer(CBaseEntity *pOther);
	virtual void DisallowPickupFor( float flDisallow ) { m_flDisallowPickup = gpGlobals->time + flDisallow; }
	virtual int ObjectCaps(void) { return CBaseEntity::ObjectCaps() | FCAP_MUST_RELEASE; }
	int AmmoToGive() const
	{
		if ( m_iDroppedOverride > 0 )
			return m_iDroppedOverride;
		return m_iAmmoToGive;
	}

	CBaseEntity *Respawn(void);
	void EXPORT Materialize(void);

	float m_flDisallowPickup = -1;
	int m_iAmountLeft = 0;
	int m_iAmmoToGive = 0;
	int m_iDroppedOverride = 0;
	char m_szSound[32];
	ZPAmmoTypes m_AmmoType = ZPAmmoTypes::AMMO_NONE;
};

extern DLL_GLOBAL short g_sModelIndexLaser; // holds the index for the laser beam
extern DLL_GLOBAL const char *g_pModelNameLaser;

extern DLL_GLOBAL short g_sModelIndexLaserDot; // holds the index for the laser beam dot
extern DLL_GLOBAL short g_sModelIndexFireball; // holds the index for the fireball
extern DLL_GLOBAL short g_sModelIndexSmoke; // holds the index for the smoke cloud
extern DLL_GLOBAL short g_sModelIndexWExplosion; // holds the index for the underwater explosion
extern DLL_GLOBAL short g_sModelIndexBubbles; // holds the index for the bubbles model
extern DLL_GLOBAL short g_sModelIndexBloodDrop; // holds the sprite index for blood drops
extern DLL_GLOBAL short g_sModelIndexBloodSpray; // holds the sprite index for blood spray (bigger)

extern void EXPORT ClearMultiDamage(void);
extern void EXPORT ApplyMultiDamage(entvars_t *pevInflictor, entvars_t *pevAttacker);
extern void EXPORT AddMultiDamage(entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType);

extern void EXPORT UTIL_PrecacheOtherWeapon(const char *szClassname);

extern void DecalGunshot(TraceResult *pTrace, int iBulletType);
extern void SpawnBlood(Vector vecSpot, int bloodColor, float flDamage);
extern int DamageDecal(CBaseEntity *pEntity, int bitsDamageType);
extern void RadiusDamage(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType);

typedef struct
{
	CBaseEntity *pEntity;
	float amount;
	int type;
} MULTIDAMAGE;

extern MULTIDAMAGE gMultiDamage;

#define LOUD_GUN_VOLUME   1000
#define NORMAL_GUN_VOLUME 600
#define QUIET_GUN_VOLUME  200

#define BRIGHT_GUN_FLASH 512
#define NORMAL_GUN_FLASH 256
#define DIM_GUN_FLASH    128

#define BIG_EXPLOSION_VOLUME    2048
#define NORMAL_EXPLOSION_VOLUME 1024
#define SMALL_EXPLOSION_VOLUME  512

#define WEAPON_ACTIVITY_VOLUME 64

#define VECTOR_CONE_1DEGREES  Vector(0.00873, 0.00873, 0.00873)
#define VECTOR_CONE_2DEGREES  Vector(0.01745, 0.01745, 0.01745)
#define VECTOR_CONE_3DEGREES  Vector(0.02618, 0.02618, 0.02618)
#define VECTOR_CONE_4DEGREES  Vector(0.03490, 0.03490, 0.03490)
#define VECTOR_CONE_5DEGREES  Vector(0.04362, 0.04362, 0.04362)
#define VECTOR_CONE_6DEGREES  Vector(0.05234, 0.05234, 0.05234)
#define VECTOR_CONE_7DEGREES  Vector(0.06105, 0.06105, 0.06105)
#define VECTOR_CONE_8DEGREES  Vector(0.06976, 0.06976, 0.06976)
#define VECTOR_CONE_9DEGREES  Vector(0.07846, 0.07846, 0.07846)
#define VECTOR_CONE_10DEGREES Vector(0.08716, 0.08716, 0.08716)
#define VECTOR_CONE_15DEGREES Vector(0.13053, 0.13053, 0.13053)
#define VECTOR_CONE_20DEGREES Vector(0.17365, 0.17365, 0.17365)

//=========================================================
// CWeaponBox - a single entity that can store weapons
// and ammo.
//=========================================================
class CWeaponBox : public CBaseEntity
{
	void Precache(void);
	void Spawn(void);
	void Touch(CBaseEntity *pOther);
	void KeyValue(KeyValueData *pkvd);
	BOOL IsEmpty(void);
	int GiveAmmo(int iCount, char *szName, int iMax, int *pIndex = NULL);
	void SetObjectCollisionBox(void);

public:
	virtual void DisallowPickupFor( float flDisallow ) { m_flDisallowPickup = gpGlobals->time + flDisallow; }
	void EXPORT Kill(void);
	int Save(CSave &save);
	int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

	BOOL HasWeapon(CBasePlayerItem *pCheckItem);
	BOOL PackWeapon(CBasePlayerItem *pWeapon);
	BOOL PackAmmo(int iszName, int iCount);

	CBasePlayerItem *m_rgpPlayerItems[MAX_ITEM_TYPES]; // one slot for each

	int m_rgiszAmmo[ZPAmmoTypes::AMMO_MAX]; // ammo names
	int m_rgAmmo[ZPAmmoTypes::AMMO_MAX]; // ammo quantities

	int m_cAmmoTypes; // how many ammo types packed into this box (if packed by a level designer)
	float m_flDisallowPickup = -1;
};

#ifdef CLIENT_DLL
bool bIsMultiplayer(void);
void LoadVModel(char *szViewModel, CBasePlayer *m_pPlayer);
#endif

class CLaserSpot : public CBaseEntity
{
	void Spawn(void);
	void Precache(void);

	int ObjectCaps(void) { return FCAP_DONT_SAVE; }

public:
	void Suspend(float flSuspendTime);
	void EXPORT Revive(void);

	static CLaserSpot *CreateSpot(void);
};

#endif // WEAPONS_H
