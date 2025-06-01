// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

enum ar556ar_e
{
	AR556_LONGIDLE = 0,
	AR556_IDLE1,
	AR556_LAUNCH,
	AR556_RELOAD,
	AR556_DEPLOY,
	AR556_FIRE1,
	AR556_FIRE2,
	AR556_FIRE3,
	AR556_HOLSTER
};

LINK_ENTITY_TO_CLASS(weapon_556ar, C556AR);
LINK_ENTITY_TO_CLASS(weapon_9mmar, C556AR); // Only for old maps, DO NOT USE THIS.

//=========================================================
//=========================================================
int C556AR::SecondaryAmmoIndex(void)
{
	return m_iSecondaryAmmoType;
}

void C556AR::Spawn()
{
	pev->classname = MAKE_STRING( "weapon_556ar" );
	Precache();
	SET_MODEL(ENT(pev), "models/w_556AR.mdl");

	WeaponData slot = GetWeaponSlotInfo( GetWeaponID() );
	m_iDefaultAmmo = slot.DefaultAmmo;

	FallInit(); // get ready to fall down.
}

void C556AR::Precache(void)
{
	PRECACHE_MODEL("models/v_556AR.mdl");
	PRECACHE_MODEL("models/w_556AR.mdl");
	PRECACHE_MODEL("models/p_556AR.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl"); // brass shellTE_MODEL

	PRECACHE_MODEL("models/grenade.mdl"); // grenade

	PRECACHE_MODEL("models/w_556ARclip.mdl");
	PRECACHE_SOUND("items/556clip1.wav");

	PRECACHE_SOUND("items/clipinsert1.wav");
	PRECACHE_SOUND("items/cliprelease1.wav");

	PRECACHE_SOUND("weapons/hks1.wav"); // H to the K
	PRECACHE_SOUND("weapons/hks2.wav"); // H to the K
	PRECACHE_SOUND("weapons/hks3.wav"); // H to the K

	PRECACHE_SOUND("weapons/glauncher.wav");
	PRECACHE_SOUND("weapons/glauncher2.wav");

	PRECACHE_SOUND("weapons/357_cock1.wav");

	m_usHUD = PRECACHE_EVENT(1, "events/m16.sc");
}

int C556AR::AddToPlayer(CBasePlayer *pPlayer)
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	{
		CBasePlayerWeapon::SendWeaponPickup(pPlayer);
		return TRUE;
	}
	return FALSE;
}

BOOL C556AR::Deploy()
{
	return DefaultDeploy("models/v_556AR.mdl", "models/p_556AR.mdl", AR556_DEPLOY, "mp5");
}

void C556AR::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
	Vector vecDir;

	vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, GetSpreadVector( PrimaryWeaponSpread() ), 8192, BULLET_PLAYER_MP5, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	int flags;
#if defined(CLIENT_WEAPONS)
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usHUD, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + PrimaryFireRate();

	if (m_flNextPrimaryAttack < UTIL_WeaponTimeBase())
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + PrimaryFireRate();

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
}

void C556AR::SecondaryAttack(void) { }

void C556AR::Reload(void)
{
	if (m_pPlayer->ammo_556ar <= 0)
		return;

	DefaultReload(AR556_MAX_CLIP, AR556_RELOAD, 1.5);
}

void C556AR::WeaponIdle(void)
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	int iAnim;
	switch (RANDOM_LONG(0, 1))
	{
	case 0:
		iAnim = AR556_LONGIDLE;
		break;

	default:
	case 1:
		iAnim = AR556_IDLE1;
		break;
	}

	SendWeaponAnim(iAnim);

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15); // how long till we do this again.
}

class C556ARAmmoClip : public CBasePlayerAmmo
{
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_556ARclip.mdl");
		CBasePlayerAmmo::Spawn();
		m_iAmountLeft = m_iAmmoToGive = AMMO_AR556CLIP_GIVE;
		m_AmmoType = ZPAmmoTypes::AMMO_RIFLE;
		strncpy(m_szSound, "items/556clip1.wav", 32);
	}
	void Precache(void)
	{
		PRECACHE_MODEL("models/w_556ARclip.mdl");
		PRECACHE_SOUND("items/556clip1.wav");
	}
};
LINK_ENTITY_TO_CLASS(ammo_556AR, C556ARAmmoClip);

class C556ARChainammo : public CBasePlayerAmmo
{
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_556box.mdl");
		CBasePlayerAmmo::Spawn();
		m_iAmountLeft = m_iAmmoToGive = AMMO_AR556BOX_GIVE;
		m_AmmoType = ZPAmmoTypes::AMMO_RIFLE;
		strncpy(m_szSound, "items/556clip1.wav", 32);
	}
	void Precache(void)
	{
		PRECACHE_MODEL("models/w_556box.mdl");
		PRECACHE_SOUND("items/556clip1.wav");
	}
};
LINK_ENTITY_TO_CLASS(ammo_556box, C556ARChainammo);
