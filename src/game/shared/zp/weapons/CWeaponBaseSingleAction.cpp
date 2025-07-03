// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "CWeaponBaseSingleAction.h"

void CWeaponBaseSingleAction::WeaponIdle( void )
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle < UTIL_WeaponTimeBase() )
	{
		if ( m_iClip == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] || m_fInSpecialReload != 0 )
			Reload();
		else
		{
			SingleActionAnimReq iAnim;
			float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
			if (flRand <= 0.8)
			{
				iAnim = ANIM_LONGIDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0 / 12.0); // * RANDOM_LONG(2, 5);
			}
			else
			{
				iAnim = ANIM_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0 / 9.0);
			}
			OnRequestedAnimation( iAnim );
		}
	}
}

void CWeaponBaseSingleAction::ItemPostFrame( void )
{
	if ( PumpIsRequired() )
	{
		WeaponPump();
		return;
	}

	CBasePlayerWeapon::ItemPostFrame();
}

void CWeaponBaseSingleAction::WeaponPump()
{
	if ( m_flNextPrimaryAttack > UTIL_WeaponTimeBase() ) return;
	m_bRequirePumping = false;

	// reload debounce has timed out
	OnRequestedAnimation( ANIM_PUMP );

	m_fInSpecialReload = 0;
}

void CWeaponBaseSingleAction::ReloadEnd()
{
	if ( m_fInSpecialReload == 0 ) return;
	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() ) return;
	OnRequestedAnimation( ANIM_RELOAD_END );
	m_fInSpecialReload = 0;
	m_bRequirePumping = false;
}

void CWeaponBaseSingleAction::Reload( void )
{
	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
	{
		ReloadEnd();
		return;
	}

	// don't reload until recoil is done
	if ( m_flNextPrimaryAttack > UTIL_WeaponTimeBase() )
		return;

	// check to see if we're ready to reload
	if ( m_fInSpecialReload == 0 )
	{
		OnRequestedAnimation( ANIM_RELOAD_START);
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.6;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
		return;
	}
	else if ( m_fInSpecialReload == 1 )
	{
		if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
			return;

		// Already full?
		if ( m_iClip == GetData().MaxClip )
		{
			ReloadEnd();
			return;
		}

		// was waiting for gun to move to side
		m_fInSpecialReload = 2;

		OnRequestedAnimation( ANIM_RELOAD );
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInSpecialReload = 1;
	}
}

void CWeaponBaseSingleAction::PrimaryAttack( void )
{
	if ( !CanPrimaryAttack() ) return;

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	OnRequestedAnimation( ANIM_PRIMARYATTACK );
	OnWeaponPrimaryAttack();

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + PrimaryFireRate();
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + PrimaryFireRate();
	if ( m_iClip != 0 )
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5.0;
	else
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;

	m_fInSpecialReload = 0;
	m_bRequirePumping = true;
}

bool CWeaponBaseSingleAction::CanPrimaryAttack()
{
	if ( PumpIsRequired() ) return false;
	if ( m_flNextPrimaryAttack > UTIL_WeaponTimeBase() ) return false;

	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return false;
	}

	// If we got no bullets, then reload.
	if ( m_iClip == 0 )
	{
		Reload();
		PlayEmptySound();
		return false;
	}

	return true;
}
