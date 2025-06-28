// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef SHARED_WEAPON_BASE_SINGLEACTION_H
#define SHARED_WEAPON_BASE_SINGLEACTION_H

#include "CWeaponBase.h"

class CWeaponBaseSingleAction : public CWeaponBase
{
	DECLARE_CLASS_SIMPLE( CWeaponBaseSingleAction, CWeaponBase );

public:
	bool IsAutomaticWeapon() const override { return false; }
	bool PumpIsRequired() const { return m_bRequirePumping; }
	virtual void WeaponIdle( void );
	virtual void ItemPostFrame( void );
	void WeaponPump();
	void ReloadEnd();
	void Reload( void ) override;
	void PrimaryAttack( void ) override;
	virtual bool CanPrimaryAttack();
	virtual void OnWeaponPrimaryAttack() {}

	// TODO: Replace this once we can figure out how to use the ACT_ crap
	// for weapons by adding new ACT table values for a custom studiomdl compiler.
	enum SingleActionAnimReq
	{
		ANIM_IDLE = 0,
		ANIM_LONGIDLE,
		ANIM_PRIMARYATTACK,
		ANIM_PUMP,
		ANIM_RELOAD_START,
		ANIM_RELOAD,
		ANIM_RELOAD_END
	};
	virtual void OnRequestedAnimation( SingleActionAnimReq act ) {}
	
protected:
	float m_flNextReload = 0.0f;

private:
	bool m_bRequirePumping = false;
};

#endif