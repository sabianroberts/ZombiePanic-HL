// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef SHARED_WEAPON_THROWABLE_SATCHEL_H
#define SHARED_WEAPON_THROWABLE_SATCHEL_H

#include "CWeaponBase.h"

class CThrowableSatchelCharge : public CGrenade
{
	Vector m_lastBounceOrigin; // Used to fix a bug in engine: when object isn't moving, but its speed isn't 0 and on ground isn't set
	void Spawn(void);
	void Precache(void);
	void BounceSound(void);

	void EXPORT SatchelSlide(CBaseEntity *pOther);
	void EXPORT SatchelThink(void);

public:
	void Deactivate(void);
};


#endif