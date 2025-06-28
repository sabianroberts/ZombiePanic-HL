// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "weapon_shotgun_remington.h"

enum
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

LINK_ENTITY_TO_CLASS( weapon_shotgun, CWeaponShotgunRemington );

void CWeaponShotgunRemington::Spawn( void )
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_shotgun.mdl");

	WeaponData slot = GetWeaponSlotInfo( GetWeaponID() );
	m_iDefaultAmmo = slot.DefaultAmmo;

	FallInit(); // get ready to fall
}

void CWeaponShotgunRemington::Precache( void )
{
	PRECACHE_MODEL( "models/v_shotgun.mdl" );
	PRECACHE_MODEL( "models/w_shotgun.mdl" );
	PRECACHE_MODEL( "models/p_shotgun.mdl" );

	PRECACHE_MODEL( "models/shotgunshell.mdl" ); // shotgun shell

	PRECACHE_SOUND( "items/9mmclip1.wav" );

	PRECACHE_SOUND( "weapons/sbarrel1.wav" ); //shotgun

	PRECACHE_SOUND( "weapons/reload1.wav" ); // shotgun reload
	PRECACHE_SOUND( "weapons/reload3.wav" ); // shotgun reload

	PRECACHE_SOUND( "weapons/357_cock1.wav" ); // gun empty sound
	PRECACHE_SOUND( "weapons/scock1.wav" ); // cock gun

	m_nEventPrimary = PRECACHE_EVENT( 1, "events/shotgun1.sc" );
}

int CWeaponShotgunRemington::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( BaseClass::AddToPlayer( pPlayer ) )
	{
		BaseClass::SendWeaponPickup( pPlayer );
		return TRUE;
	}
	return FALSE;
}

BOOL CWeaponShotgunRemington::Deploy()
{
	return DefaultDeploy( "models/v_shotgun.mdl", "models/p_shotgun.mdl", SHOTGUN_DRAW, "shotgun" );
}

void CWeaponShotgunRemington::OnRequestedAnimation( SingleActionAnimReq act )
{
	switch ( act )
	{
		case CWeaponBaseSingleAction::ANIM_IDLE: SendWeaponAnim( SHOTGUN_IDLE ); break;
		case CWeaponBaseSingleAction::ANIM_LONGIDLE: SendWeaponAnim( SHOTGUN_IDLE2 ); break;
		//case CWeaponBaseSingleAction::ANIM_PRIMARYATTACK: SendWeaponAnim( SHOTGUN_FIRE ); break;
		case CWeaponBaseSingleAction::ANIM_PUMP:
		{
			// reload debounce has timed out
			SendWeaponAnim( SHOTGUN_PUMP );
			// play cocking sound
			EMIT_SOUND_DYN( ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 105 );
			m_flTimeWeaponIdle = m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5f;
		}
		break;
		case CWeaponBaseSingleAction::ANIM_RELOAD_START: SendWeaponAnim( SHOTGUN_RELOAD_START ); break;
		case CWeaponBaseSingleAction::ANIM_RELOAD:
		{
		    if (RANDOM_LONG(0, 1))
			    EMIT_SOUND_DYN( ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f) );
		    else
			    EMIT_SOUND_DYN( ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload3.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f) );

		    SendWeaponAnim( SHOTGUN_RELOAD );

		    m_flNextReload = UTIL_WeaponTimeBase() + 0.5;
		    m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
		}
		break;
		case CWeaponBaseSingleAction::ANIM_RELOAD_END:
		{
			SendWeaponAnim( SHOTGUN_RELOAD_END );
			EMIT_SOUND_DYN( ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 105 );
			m_flNextReload = UTIL_WeaponTimeBase() + 1.5;
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;
		}
		break;
	}
}

void CWeaponShotgunRemington::OnWeaponPrimaryAttack()
{
	int flags;
#if defined(CLIENT_WEAPONS)
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
	Vector vecDir;
	vecDir = m_pPlayer->FireBulletsPlayer(4, vecSrc, vecAiming, GetSpreadVector( PrimaryWeaponSpread() ), 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_nEventPrimary, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );
}


class CShotgunAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{
		Precache();
		SET_MODEL( ENT(pev), "models/w_shotbox.mdl" );
		CBasePlayerAmmo::Spawn();
		m_iAmountLeft = m_iAmmoToGive = AMMO_BUCKSHOTBOX_GIVE;
		m_AmmoType = ZPAmmoTypes::AMMO_SHOTGUN;
		strncpy( m_szSound, "items/9mmclip1.wav", 32 );
	}
	void Precache( void )
	{
		PRECACHE_MODEL( "models/w_shotbox.mdl" );
		PRECACHE_SOUND( "items/9mmclip1.wav" );
	}
};
LINK_ENTITY_TO_CLASS( ammo_buckshot, CShotgunAmmo );
