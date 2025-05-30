// ============== Copyright (c) 2025 Monochrome Games ============== \\

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"
#include "zp/zp_shared.h"

#define SWIPE_BODYHIT_VOLUME 128
#define SWIPE_WALLHIT_VOLUME 512

LINK_ENTITY_TO_CLASS(weapon_swipe, CSwipe);

enum animtable_e
{
	SWIPE_IDLE = 0,
	SWIPE_DRAW,
	SWIPE_HOLSTER,
	SWIPE_ATTACK1HIT,
	SWIPE_ATTACK1MISS,
	SWIPE_ATTACK2MISS,
	SWIPE_ATTACK2HIT,
	SWIPE_ATTACK3MISS,
	SWIPE_ATTACK3HIT
};

void CSwipe::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_swipe.mdl");
	m_iClip = -1;
	pev->team = ZP::TEAM_ZOMBIE;

	FallInit(); // get ready to fall down.
}

void CSwipe::Precache(void)
{
	PRECACHE_MODEL("models/v_swipe.mdl");
	PRECACHE_MODEL("models/w_swipe.mdl");
	PRECACHE_MODEL("models/p_swipe.mdl");
	PRECACHE_SOUND("weapons/claw_strike1.wav");
	PRECACHE_SOUND("weapons/claw_strike2.wav");
	PRECACHE_SOUND("weapons/claw_strike3.wav");
	PRECACHE_SOUND("weapons/claw_miss1.wav");
	PRECACHE_SOUND("weapons/claw_miss2.wav");

	m_usCrowbar = PRECACHE_EVENT(1, "events/swipe.sc");
}

int CSwipe::AddToPlayer(CBasePlayer *pPlayer)
{
	if ( pPlayer->pev->team != ZP::TEAM_ZOMBIE )
	{
		UTIL_Remove( this );
		return FALSE;
	}
	if (CBasePlayerWeapon::AddToPlayer(pPlayer))
	{
		CBasePlayerWeapon::SendWeaponPickup(pPlayer);
		return TRUE;
	}
	UTIL_Remove( this );
	return FALSE;
}

BOOL CSwipe::Deploy()
{
	return DefaultDeploy("models/v_swipe.mdl", "models/p_swipe.mdl", SWIPE_DRAW, "crowbar");
}

void CSwipe::Holster(int skiplocal /* = 0 */)
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim(SWIPE_HOLSTER);
}

extern void FindHullIntersection(const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity);

void CSwipe::PrimaryAttack()
{
	if (!Swing(1))
	{
		SetThink(&CSwipe::SwingAgain);
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CSwipe::Smack()
{
	DecalGunshot(&m_trHit, BULLET_PLAYER_CROWBAR);
}

void CSwipe::SwingAgain(void)
{
	Swing(0);
}

int CSwipe::Swing(int fFirst)
{
	int fDidHit = FALSE;

	TraceResult tr;

	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

#ifndef CLIENT_DLL
	if (tr.flFraction >= 1.0)
	{
		UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);
		if (tr.flFraction < 1.0)
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
			if (!pHit || pHit->IsBSPModel())
				FindHullIntersection(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict());
			vecEnd = tr.vecEndPos; // This is the point on the actual surface (the hull could have hit space)
		}
	}
#endif

	if (fFirst)
	{
		PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_usCrowbar,
		    0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, 0,
		    0.0, 0, 0.0);
	}

	if (tr.flFraction >= 1.0)
	{
		if (fFirst)
		{
			// miss
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;

			// player "shoot" animation
			m_pPlayer->SetAnimation(PLAYER_ATTACK1);
		}
	}
	else
	{
		switch (((m_iSwing++) % 2) + 1)
		{
		case 0:
			SendWeaponAnim(SWIPE_ATTACK1HIT);
			break;
		case 1:
			SendWeaponAnim(SWIPE_ATTACK2HIT);
			break;
		case 2:
			SendWeaponAnim(SWIPE_ATTACK3HIT);
			break;
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);

#ifndef CLIENT_DLL

		// hit
		fDidHit = TRUE;
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		ClearMultiDamage();

		// JoshA: Changed from < -> <= to fix the full swing logic since client weapon prediction.
		// -1.0f + 1.0f = 0.0f. UTIL_WeaponTimeBase is always 0 with client weapon prediction (0 time base vs curtime base)
		if ((m_flNextPrimaryAttack + 1.0f <= UTIL_WeaponTimeBase()) || g_pGameRules->IsMultiplayer())
		{
			// first swing does full damage
			pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgSwipe, gpGlobals->v_forward, &tr, DMG_CLUB);
		}
		else
		{
			// subsequent swings do half
			pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgSwipe / 2, gpGlobals->v_forward, &tr, DMG_CLUB);
		}
		ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;

		if (pEntity)
		{
			if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
			{
				// play thwack or smack sound
				switch (RANDOM_LONG(0, 2))
				{
				case 0:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/claw_strike1.wav", 1, ATTN_NORM);
					break;
				case 1:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/claw_strike2.wav", 1, ATTN_NORM);
					break;
				case 2:
					EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/claw_strike3.wav", 1, ATTN_NORM);
					break;
				}
				m_pPlayer->m_iWeaponVolume = SWIPE_BODYHIT_VOLUME;
				if (!pEntity->IsAlive())
					return TRUE;
				else
					flVol = 0.1;

				fHitWorld = FALSE;
			}
		}

		// play texture hit sound
		// UNDONE: Calculate the correct point of intersection when we hit with the hull instead of the line

		if (fHitWorld)
		{
			float fvolbar = TEXTURETYPE_PlaySound(&tr, vecSrc, vecSrc + (vecEnd - vecSrc) * 2, BULLET_PLAYER_CROWBAR);

			if (g_pGameRules->IsMultiplayer())
			{
				// override the volume here, cause we don't play texture sounds in multiplayer,
				// and fvolbar is going to be 0 from the above call.

				fvolbar = 1;
			}

			// also play crowbar strike
			switch (RANDOM_LONG(0, 2))
			{
			case 0:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/claw_strike1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));
				break;
			case 1:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/claw_strike2.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));
				break;
			case 2:
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/claw_strike3.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));
				break;
			}

			// delay the decal a bit
			m_trHit = tr;
		}

		m_pPlayer->m_iWeaponVolume = flVol * SWIPE_WALLHIT_VOLUME;
#endif
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + PrimaryFireRate();

		SetThink(&CSwipe::Smack);
		pev->nextthink = gpGlobals->time + 0.2;
	}
	return fDidHit;
}
