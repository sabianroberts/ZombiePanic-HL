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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"

// special deathmatch shotgun spreads
#define VECTOR_CONE_DM_SHOTGUN       Vector(0.08716, 0.04362, 0.00) // 10 degrees by 5 degrees
#define VECTOR_CONE_DM_DOUBLESHOTGUN Vector(0.17365, 0.04362, 0.00) // 20 degrees by 5 degrees

enum shotgun_e
{
	SHOTGUN_IDLE = 0,
	SHOTGUN_IDLE2,
	SHOTGUN_FIRE,
	SHOTGUN_PUMP,
	SHOTGUN_RELOAD_START,
	SHOTGUN_RELOAD,
	SHOTGUN_RELOAD_END,
	SHOTGUN_DRAW
};

LINK_ENTITY_TO_CLASS(weapon_shotgun, CShotgun);

void CShotgun::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_shotgun.mdl");

	WeaponData slot = GetWeaponSlotInfo( GetWeaponID() );
	m_iDefaultAmmo = slot.DefaultAmmo;
	m_bRequirePump = false;

	FallInit(); // get ready to fall
}

void CShotgun::Precache(void)
{
	PRECACHE_MODEL("models/v_shotgun.mdl");
	PRECACHE_MODEL("models/w_shotgun.mdl");
	PRECACHE_MODEL("models/p_shotgun.mdl");

	m_iShell = PRECACHE_MODEL("models/shotgunshell.mdl"); // shotgun shell

	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("weapons/dbarrel1.wav"); //shotgun
	PRECACHE_SOUND("weapons/sbarrel1.wav"); //shotgun

	PRECACHE_SOUND("weapons/reload1.wav"); // shotgun reload
	PRECACHE_SOUND("weapons/reload3.wav"); // shotgun reload

	//	PRECACHE_SOUND ("weapons/sshell1.wav");	// shotgun reload - played on client
	//	PRECACHE_SOUND ("weapons/sshell3.wav");	// shotgun reload - played on client

	PRECACHE_SOUND("weapons/357_cock1.wav"); // gun empty sound
	PRECACHE_SOUND("weapons/scock1.wav"); // cock gun

	m_usSingleFire = PRECACHE_EVENT(1, "events/shotgun1.sc");
	m_usDoubleFire = PRECACHE_EVENT(1, "events/shotgun2.sc");
}

int CShotgun::AddToPlayer(CBasePlayer *pPlayer)
{
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	{
		CBasePlayerWeapon::SendWeaponPickup(pPlayer);
		return TRUE;
	}
	return FALSE;
}

BOOL CShotgun::Deploy()
{
	return DefaultDeploy("models/v_shotgun.mdl", "models/p_shotgun.mdl", SHOTGUN_DRAW, "shotgun");
}

void CShotgun::ReloadEnd()
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) return;
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase()) return;
	SendWeaponAnim(SHOTGUN_RELOAD_END);
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 105);
	m_fInSpecialReload = 0;
	m_bRequirePump = false;
	m_flNextReload = UTIL_WeaponTimeBase() + 1.5;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;
}

void CShotgun::PrimaryAttack()
{
	// We need to pump? Stop!
	if ( m_bRequirePump ) return;

	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if (m_iClip <= 0)
	{
		Reload();
		if (m_iClip == 0)
			PlayEmptySound();
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	int flags;
#if defined(CLIENT_WEAPONS)
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	Vector vecDir;

	vecDir = m_pPlayer->FireBulletsPlayer(4, vecSrc, vecAiming, GetSpreadVector( PrimaryWeaponSpread() ), 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usSingleFire, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + PrimaryFireRate();
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + PrimaryFireRate();
	if (m_iClip != 0)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5.0;
	else
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
	m_fInSpecialReload = 0;
	m_bRequirePump = true;
}

void CShotgun::SecondaryAttack(void)
{
}

void CShotgun::Reload(void)
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		ReloadEnd();
		return;
	}

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return;

	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		SendWeaponAnim(SHOTGUN_RELOAD_START);
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.6;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
		return;
	}
	else if (m_fInSpecialReload == 1)
	{
		if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return;

		// Already full?
		if ( m_iClip == GetData().MaxClip )
		{
			ReloadEnd();
			return;
		}

		// was waiting for gun to move to side
		m_fInSpecialReload = 2;

		if (RANDOM_LONG(0, 1))
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f));
		else
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload3.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f));

		SendWeaponAnim(SHOTGUN_RELOAD);

		m_flNextReload = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInSpecialReload = 1;
	}
}

void CShotgun::WeaponIdle(void)
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
	{
		if (m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] || m_fInSpecialReload != 0)
			Reload();
		else
		{
			int iAnim;
			float flRand = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0, 1);
			if (flRand <= 0.8)
			{
				iAnim = SHOTGUN_IDLE2;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0 / 12.0); // * RANDOM_LONG(2, 5);
			}
			else
			{
				iAnim = SHOTGUN_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0 / 9.0);
			}
			SendWeaponAnim(iAnim);
		}
	}
}

void CShotgun::WeaponPump()
{
	if ( m_flNextPrimaryAttack > UTIL_WeaponTimeBase() ) return;
	m_bRequirePump = false;

	// reload debounce has timed out
	SendWeaponAnim(SHOTGUN_PUMP);

	// play cocking sound
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 105);
	m_fInSpecialReload = 0;
	m_flTimeWeaponIdle = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + PrimaryFireRate() + 0.2f;
}

void CShotgun::ItemPostFrame(void)
{
	if ( m_bRequirePump )
	{
		WeaponPump();
		return;
	}

	CBasePlayerWeapon::ItemPostFrame();
}

class CShotgunAmmo : public CBasePlayerAmmo
{
	void Spawn(void)
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_shotbox.mdl");
		CBasePlayerAmmo::Spawn();
		m_iAmountLeft = m_iAmmoToGive = AMMO_BUCKSHOTBOX_GIVE;
		m_AmmoType = ZPAmmoTypes::AMMO_SHOTGUN;
		strncpy( m_szSound, "items/9mmclip1.wav", 32 );
	}
	void Precache(void)
	{
		PRECACHE_MODEL("models/w_shotbox.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
};
LINK_ENTITY_TO_CLASS(ammo_buckshot, CShotgunAmmo);
