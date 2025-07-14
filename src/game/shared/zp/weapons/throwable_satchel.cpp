// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "throwable_satchel.h"


LINK_ENTITY_TO_CLASS(monster_satchel, CThrowableSatchelCharge);

void CThrowableSatchelCharge::Deactivate(void)
{
	pev->solid = SOLID_NOT;
	UTIL_Remove(this);
}

void CThrowableSatchelCharge::Spawn(void)
{
	Precache();
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/w_satchel.mdl");
	//UTIL_SetSize(pev, Vector( -16, -16, -4), Vector(16, 16, 32));	// Old box -- size of headcrab monsters/players get blocked by this
	UTIL_SetSize(pev, Vector(-4, -4, -4), Vector(4, 4, 4)); // Uses point-sized, and can be stepped over
	UTIL_SetOrigin(pev, pev->origin);

	SetTouch(&CThrowableSatchelCharge::SatchelSlide);
	SetUse(&CThrowableSatchelCharge::SatchelUse);
	SetThink(&CThrowableSatchelCharge::SatchelThink);
	pev->nextthink = gpGlobals->time + 0.1;

	pev->team = ZP::TEAM_SURVIVIOR;
	pev->gravity = 0.5;
	pev->friction = 0.8;

	pev->dmg = gSkillData.plrDmgSatchel;
	// ResetSequenceInfo( );
	pev->sequence = 1;
	m_flExplodeRange = 300.0f;
}

void CThrowableSatchelCharge::SatchelSlide(CBaseEntity *pOther)
{
	entvars_t *pevOther = pOther->pev;

	// Allow for pickup! (if we can)
	if ( m_iThrower != -1 && pOther->IsPlayer() && pOther->pev->team == ZP::TEAM_SURVIVIOR )
	{
#ifndef CLIENT_DLL
		CBasePlayer *pOwner = (CBasePlayer *)UTIL_PlayerByIndex( m_iThrower );
		bool bCanBePickedUp = pOwner ? false : true;
		if ( pOwner && pOwner->pev->team != pev->team )
			bCanBePickedUp = true;
		else if ( pOther->edict() == pOwner->edict() )
			bCanBePickedUp = true;
		if ( bCanBePickedUp )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)pOther;
			pPlayer->GiveNamedItem( "weapon_satchel" );
			SetTouch( NULL );
			SetThink( &CThrowableSatchelCharge::SUB_Remove );
			return;
		}
#endif
	}

	// don't hit the guy that launched this grenade
	if (pOther->edict() == pev->owner)
		return;

	// pev->avelocity = Vector (300, 300, 300);
	pev->gravity = 1; // normal gravity now

	// HACKHACK - On ground isn't always set, so look for ground underneath
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin - Vector(0, 0, 10), ignore_monsters, edict(), &tr);

	if (tr.flFraction < 1.0)
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.95;
		pev->avelocity = pev->avelocity * 0.9;
		// play sliding sound, volume based on velocity
	}
	if (!(pev->flags & FL_ONGROUND) && pev->velocity.Length2D() > 10)
	{
		// Fix for a bug in engine: when object isn't moving, but its speed isn't 0 and on ground isn't set
		if (pev->origin != m_lastBounceOrigin)
		{
			BounceSound();
		}
	}
	m_lastBounceOrigin = pev->origin;
	// There is no model animation so commented this out to prevent net traffic
	//StudioFrameAdvance( );
}

void CThrowableSatchelCharge ::SatchelThink(void)
{
	// There is no model animation so commented this out to prevent net traffic
	//StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1;

	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	if (pev->waterlevel == 3)
	{
		pev->movetype = MOVETYPE_FLY;
		pev->velocity = pev->velocity * 0.8;
		pev->avelocity = pev->avelocity * 0.9;
		pev->velocity.z += 8;
	}
	else if (pev->waterlevel == 0)
	{
		pev->movetype = MOVETYPE_BOUNCE;
	}
	else
	{
		pev->velocity.z -= 8;
	}

	if ( m_flDisallowPickup != -1 && m_flDisallowPickup - gpGlobals->time <= 0 )
	{
		m_flDisallowPickup = -1;
		m_iThrower = pev->owner ? ENTINDEX( pev->owner ) : 0;
		pev->owner = nullptr;
	}
}

void CThrowableSatchelCharge::SatchelUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if ( useType == USE_SET )
	{
#ifndef CLIENT_DLL
		if ( m_iThrower != -1 )
		{
			CBasePlayer *pOwner = (CBasePlayer *)UTIL_PlayerByIndex( m_iThrower );
			pev->owner = pOwner->edict();
		}
#endif
		CGrenade::DetonateUse( pActivator, pCaller, useType, value );
		return;
	}
}

void CThrowableSatchelCharge ::Precache(void)
{
	PRECACHE_MODEL("models/w_satchel.mdl");
	PRECACHE_SOUND("weapons/g_bounce1.wav");
	PRECACHE_SOUND("weapons/g_bounce2.wav");
	PRECACHE_SOUND("weapons/g_bounce3.wav");
}

void CThrowableSatchelCharge ::BounceSound(void)
{
	switch (RANDOM_LONG(0, 2))
	{
	case 0:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce1.wav", 1, ATTN_NORM);
		break;
	case 1:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce2.wav", 1, ATTN_NORM);
		break;
	case 2:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/g_bounce3.wav", 1, ATTN_NORM);
		break;
	}
}