/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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
//
// Ammo.cpp
//
// implementation of CHudAmmo class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "pm_shared.h"

#include <string.h>
#include <stdio.h>

#include <vgui/ISurface.h>

#include "ammo.h"
#include "ammohistory.h"
#include "crosshair.h"
#include "menu.h"
#include "vgui/client_viewport.h"

ConVar hud_fastswitch("hud_fastswitch", "0", FCVAR_ARCHIVE, "Controls whether or not weapons can be selected in one keypress");
ConVar hud_weapon("hud_weapon", "0", FCVAR_BHL_ARCHIVE, "Controls displaying sprite of currently selected weapon");
extern ConVar cl_cross_zoom;

WEAPON *gpActiveSel; // NULL means off, 1 means just the menu bar, otherwise
    // this points to the active weapon menu item
WEAPON *gpLastSel; // Last weapon menu selection

client_sprite_t *GetSpriteFromList(client_sprite_t *pList, const char *psz, int iRes, int iCount);

WeaponsResource gWR;

// Used by zp_ammobank.cpp
int g_ActiveAmmoIndex = 0;

int g_weaponselect = 0;

#define SET_WEAPON_SELECTION_XPOS 180
#define SET_WEAPON_SELECTION_YPOS 20

void WeaponsResource::LoadAllWeaponSprites(void)
{
	for (int i = 0; i < MAX_WEAPONS; i++)
	{
		if (rgWeapons[i].iId)
			LoadWeaponSprites(&rgWeapons[i]);
	}
}

int WeaponsResource::CountAmmo(int iId)
{
	if (iId < 0)
		return 0;

	return riAmmo[iId];
}

int WeaponsResource::HasAmmo(WEAPON *p)
{
	if (!p)
		return FALSE;

	// We always return true if the weapon is valid.
	// Because we want to drop the weapon if we have no ammo etc.
	return TRUE;
}

void WeaponsResource::LoadWeaponSprites(WEAPON *pWeapon)
{
	int i = 0;
	int iRes = gHUD.m_iRes;
	char sz[256];
	if ( !pWeapon ) return;

	WeaponData &wpnDat = GetWeaponSlotInfo( pWeapon->szName );
	pWeapon->hInactive = gHUD.GetRegisteredIcon( wpnDat.Icons[ICON_WEAPON] );
	pWeapon->hActive = gHUD.GetRegisteredIcon( wpnDat.Icons[ICON_WEAPON_SELECTED] );
	pWeapon->hAmmo = gHUD.GetRegisteredIcon( wpnDat.Icons[ICON_AMMO1] );
	pWeapon->hAmmo2 = gHUD.GetRegisteredIcon( wpnDat.Icons[ICON_AMMO2] );
	pWeapon->hCrosshair = gHUD.GetRegisteredIcon( wpnDat.Icons[ICON_CROSSHAIR] );
	pWeapon->hAutoaim = gHUD.GetRegisteredIcon( wpnDat.Icons[ICON_CROSSHAIR_AUTO] );
	pWeapon->hZoomedCrosshair = gHUD.GetRegisteredIcon( wpnDat.Icons[ICON_CROSSHAIR_ZOOM] );
	pWeapon->hZoomedAutoaim = gHUD.GetRegisteredIcon( wpnDat.Icons[ICON_CROSSHAIR_ZOOMAUTO] );
}

// Returns the first weapon for a given slot.
WEAPON *WeaponsResource::GetFirstPos(int iSlot)
{
	if (iSlot >= MAX_WEAPON_SLOTS)
		return NULL;

	WEAPON *pret = NULL;

	for (int i = 0; i < MAX_WEAPON_POSITIONS; i++)
	{
		if (rgSlots[iSlot][i] && HasAmmo(rgSlots[iSlot][i]))
		{
			pret = rgSlots[iSlot][i];
			break;
		}
	}

	return pret;
}

WEAPON *WeaponsResource::GetNextActivePos(int iSlot, int iSlotPos)
{
	if (iSlot >= MAX_WEAPON_SLOTS || iSlotPos + 1 >= MAX_WEAPON_POSITIONS)
		return NULL;

	WEAPON *p = gWR.rgSlots[iSlot][iSlotPos + 1];

	if (!p || !gWR.HasAmmo(p))
		return GetNextActivePos(iSlot, iSlotPos + 1);

	return p;
}

int giBucketHeight, giBucketWidth, giABHeight, giABWidth; // Ammo Bar width and height

V_HSPRITE ghsprBuckets; // Sprite for top row of weapons menu

// width of ammo fonts
#define AMMO_SMALL_WIDTH 10
#define AMMO_LARGE_WIDTH 20

#define HISTORY_DRAW_TIME "5"

DEFINE_HUD_ELEM(CHudAmmo);

void CHudAmmo::Init()
{
	BaseHudClass::Init();

	HookMessage<&CHudAmmo::MsgFunc_CurWeapon>("CurWeapon"); // Current weapon and clip
	HookMessage<&CHudAmmo::MsgFunc_WeaponList>("WeaponList"); // new weapon type
	HookMessage<&CHudAmmo::MsgFunc_AmmoPickup>("AmmoPickup"); // flashes an ammo pickup record
	HookMessage<&CHudAmmo::MsgFunc_WeapPickup>("WeapPickup"); // flashes a weapon pickup record
	HookMessage<&CHudAmmo::MsgFunc_ItemPickup>("ItemPickup");
	HookMessage<&CHudAmmo::MsgFunc_HideWeapon>("HideWeapon"); // hides the weapon, ammo, and crosshair displays temporarily
	HookMessage<&CHudAmmo::MsgFunc_AmmoX>("AmmoX"); // update known ammo type's count

	HookCommand<&CHudAmmo::UserCmd_Slot1>("slot1");
	HookCommand<&CHudAmmo::UserCmd_Slot2>("slot2");
	HookCommand<&CHudAmmo::UserCmd_Slot3>("slot3");
	HookCommand<&CHudAmmo::UserCmd_Slot4>("slot4");
	HookCommand<&CHudAmmo::UserCmd_Slot5>("slot5");
	HookCommand<&CHudAmmo::UserCmd_Slot6>("slot6");
	HookCommand<&CHudAmmo::UserCmd_Slot7>("slot7");
	HookCommand<&CHudAmmo::UserCmd_Slot8>("slot8");
	HookCommand<&CHudAmmo::UserCmd_Slot9>("slot9");
	HookCommand<&CHudAmmo::UserCmd_Slot10>("slot10");
	HookCommand<&CHudAmmo::UserCmd_Close>("cancelselect");
	HookCommand<&CHudAmmo::UserCmd_NextWeapon>("invnext");
	HookCommand<&CHudAmmo::UserCmd_PrevWeapon>("invprev");

	Reset();

	m_iFlags |= HUD_ACTIVE; //!!!

	gWR.Init();
	gHR.Init();
};

void CHudAmmo::Reset(void)
{
	m_fFade = 0;
	g_ActiveAmmoIndex = -1;
	m_pWeapon = NULL;
	m_fOnTarget = false;
	m_iFlags |= HUD_ACTIVE; //!!!

	gpActiveSel = NULL;
	gHUD.m_iHideHUDDisplay = 0;

	gWR.Reset();
	gHR.Reset();

	//	VidInit();

	m_iMaxSlot = 4;
}

void CHudAmmo::VidInit()
{
	// Load sprites for buckets (top row of weapon menu)
	m_HUD_bucket0 = gHUD.GetSpriteIndex("bucket1");
	m_HUD_selection = gHUD.GetSpriteIndex("selection");

	ghsprBuckets = gHUD.GetSprite(m_HUD_bucket0);
	giBucketWidth = gHUD.GetSpriteRect(m_HUD_bucket0).right - gHUD.GetSpriteRect(m_HUD_bucket0).left;
	giBucketHeight = gHUD.GetSpriteRect(m_HUD_bucket0).bottom - gHUD.GetSpriteRect(m_HUD_bucket0).top;

	gHR.iHistoryGap = max(gHR.iHistoryGap, gHUD.GetSpriteRect(m_HUD_bucket0).bottom - gHUD.GetSpriteRect(m_HUD_bucket0).top);

	// If we've already loaded weapons, let's get new sprites
	gWR.LoadAllWeaponSprites();

	giABWidth = SPR_RES_SCALED(20);
	giABHeight = SPR_RES_SCALED(4);
}

//
// Think:
//  Used for selection of weapon menu item.
//
void CHudAmmo::Think(void)
{
	if (gHUD.m_iFOV != m_iLastFOV)
		m_iLastFOV = gHUD.m_iFOV;

	if (gHUD.m_fPlayerDead)
		return;

	if (gHUD.m_iWeaponBits != gWR.iOldWeaponBits)
	{
		gWR.iOldWeaponBits = gHUD.m_iWeaponBits;

		for (int i = MAX_WEAPONS - 1; i > 0; i--)
		{
			WEAPON *p = gWR.GetWeapon(i);

			if (p)
			{
				if (gHUD.m_iWeaponBits & (1 << p->iId))
					gWR.PickupWeapon(p);
				else
					gWR.DropWeapon(p);
			}
		}
	}

	if (!gpActiveSel)
		return;

	// has the player selected one?
	if (gHUD.m_iKeyBits & IN_ATTACK || hud_fastswitch.GetInt() == 2)
	{
		if (gpActiveSel != (WEAPON *)1)
		{
			ServerCmd(gpActiveSel->szName);
			g_weaponselect = gpActiveSel->iId;
		}

		gpLastSel = gpActiveSel;
		gpActiveSel = NULL;
		gHUD.m_iKeyBits &= ~IN_ATTACK;

		PlaySound("common/wpn_select.wav", 1);
	}
}

//
// Helper function to return a Ammo pointer from id
//

CHud::RegisteredIcon WeaponsResource::GetAmmoPicFromWeapon(int iAmmoId)
{
	for (int i = 0; i < MAX_WEAPONS; i++)
	{
		if (rgWeapons[i].iAmmoType == iAmmoId)
			return rgWeapons[i].hAmmo;
		else if (rgWeapons[i].iAmmo2Type == iAmmoId)
			return rgWeapons[i].hAmmo2;
	}
	return CHud::RegisteredIcon();
}

// Menu Selection Code

void WeaponsResource::SelectSlot(int iSlot, int fAdvance, int iDirection)
{
	if ((fAdvance == FALSE) && (iDirection == 1) && CHudMenu::Get()->OnWeaponSlotSelected(iSlot))
	{
		// menu is overriding slot use commands
		return;
	}

	if (iSlot > CHudAmmo::Get()->GetMaxSlot())
		return;

	if (gHUD.m_fPlayerDead || gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL))
		return;

	if (!(gHUD.m_iWeaponBits & (1 << (WEAPON_SUIT))))
		return;

	if (!(gHUD.m_iWeaponBits & ~(1 << (WEAPON_SUIT))))
		return;

	WEAPON *p = NULL;
	bool fastSwitch = hud_fastswitch.GetInt() != 0;

	if ((gpActiveSel == NULL) || (gpActiveSel == (WEAPON *)1) || (iSlot != gpActiveSel->iSlot))
	{
		PlaySound("common/wpn_hudon.wav", 1);
		p = GetFirstPos(iSlot);

		if (p && fastSwitch) // check for fast weapon switch mode
		{
			// if fast weapon switch is on, then weapons can be selected in a single keypress
			// but only if there is only one item in the bucket
			WEAPON *p2 = GetNextActivePos(p->iSlot, p->iSlotPos);
			if (!p2)
			{ // only one active item in bucket, so change directly to weapon
				ServerCmd(p->szName);
				g_weaponselect = p->iId;
				return;
			}
		}
	}
	else
	{
		PlaySound("common/wpn_moveselect.wav", 1);
		if (gpActiveSel)
			p = GetNextActivePos(gpActiveSel->iSlot, gpActiveSel->iSlotPos);
		if (!p)
			p = GetFirstPos(iSlot);
	}

	if (!p) // no selection found
	{
		// just display the weapon list, unless fastswitch is on just ignore it
		if (!fastSwitch)
			gpActiveSel = (WEAPON *)1;
		else
			gpActiveSel = NULL;
	}
	else
		gpActiveSel = p;
}

//------------------------------------------------------------------------
// Message Handlers
//------------------------------------------------------------------------

//
// AmmoX  -- Update the count of a known type of ammo
//
int CHudAmmo::MsgFunc_AmmoX(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iIndex = READ_BYTE();
	int iCount = READ_BYTE();

	gWR.SetAmmo(iIndex, abs(iCount));

	m_fFade = FADE_TIME;

	return 1;
}

int CHudAmmo::MsgFunc_AmmoPickup(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_BYTE();

	// Ammo Index
	AmmoData data = GetAmmoByAmmoID( iIndex );

	// Add ammo to the history
	gHR.AddToHistory(HISTSLOT_AMMO, data.AmmoName, abs(iCount));

	return 1;
}

int CHudAmmo::MsgFunc_WeapPickup(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();

	// Weapon Index
	WeaponData data = GetWeaponSlotInfo( (ZPWeaponID)iIndex );

	// Add the weapon to the history
	gHR.AddToHistory( HISTSLOT_WEAP, data.Icons[WeaponDataIcons::ICON_WEAPON] );

	return 1;
}

int CHudAmmo::MsgFunc_ItemPickup(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	const char *szName = READ_STRING();

	// Add the weapon to the history
	gHR.AddToHistory(HISTSLOT_ITEM, szName);

	return 1;
}

int CHudAmmo::MsgFunc_HideWeapon(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	gHUD.m_iHideHUDDisplay = READ_BYTE();

	if (gEngfuncs.IsSpectateOnly())
		return 1;

	if (gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL))
		gpActiveSel = NULL;

	static wrect_t nullrc;
	SetCrosshair(0, nullrc, 0, 0, 0);

	return 1;
}

bool CHudAmmo::IsIconValid(CHud::RegisteredIcon icon)
{
	return ( icon.Icon > -1 ) ? true : false;
}

//
//  CurWeapon: Update hud state with the current weapon and clip count. Ammo
//  counts are updated with AmmoX. Server assures that the Weapon ammo type
//  numbers match a real ammo type.
//
int CHudAmmo::MsgFunc_CurWeapon(const char *pszName, int iSize, void *pbuf)
{
	static wrect_t nullrc;

	BEGIN_READ(pbuf, iSize);

	int iState = READ_BYTE();
	int iId = READ_CHAR();
	int iClip = READ_CHAR();
	int iMaxClip = READ_CHAR();

	// detect if we're also on target
	m_fOnTarget = iState > 1;

	if (iId < 1)
	{
		g_ActiveAmmoIndex = -1;
		m_pWeapon = NULL;
		SetCrosshair(0, nullrc, 0, 0, 0);
		return 0;
	}

	if (g_iUser1 != OBS_IN_EYE)
	{
		// Is player dead???
		if ((iId == -1) && (iClip == -1))
		{
			gHUD.m_fPlayerDead = TRUE;
			gpActiveSel = NULL;
			return 1;
		}
		gHUD.m_fPlayerDead = FALSE;
	}

	WEAPON *pWeapon = gWR.GetWeapon(iId);

	if (!pWeapon)
		return 0;

	if (iClip < -1)
		pWeapon->iClip = abs(iClip);
	else
		pWeapon->iClip = iClip;

	if (iMaxClip < -1)
		pWeapon->iMaxClip = abs(iMaxClip);
	else
		pWeapon->iMaxClip = iMaxClip;

	if (iState == 0) // we're not the current weapon, so update no more
		return 1;

	m_pWeapon = pWeapon;
	g_ActiveAmmoIndex = m_pWeapon->iAmmoType;

	SetCrosshair( 0, nullrc, 0, 0, 0 );

	m_fFade = FADE_TIME;
	m_iFlags |= HUD_ACTIVE;

	return 1;
}

void CHudAmmo::DrawCrosshair()
{
	static wrect_t nullrc;
	if ( m_pWeapon == NULL )
	{
		SetCrosshair( 0, nullrc, 0, 0, 0 );
		return;
	}
	if ( CHudCrosshair::Get()->IsEnabled() ) return;

	int y = ScreenHeight / 2;
	int x = ScreenWidth / 2;
	Color COLOR_WHITE( 255, 255, 255, 255 );

	if ( gHUD.m_iFOV >= 90 )
	{
		if ( IsIconValid( m_pWeapon->hAutoaim ) && m_fOnTarget )
			DrawCrosshair( m_pWeapon->hAutoaim, x, y, COLOR_WHITE );
		else
			DrawCrosshair( m_pWeapon->hCrosshair, x, y, COLOR_WHITE );
	}
	else
	{
		if ( m_fOnTarget && IsIconValid( m_pWeapon->hZoomedAutoaim ) )
			DrawCrosshair( m_pWeapon->hZoomedAutoaim, x, y, COLOR_WHITE );
		else
			DrawCrosshair( m_pWeapon->hZoomedCrosshair, x, y, COLOR_WHITE );
	}
}

void CHudAmmo::DrawCrosshair( CHud::RegisteredIcon icon, int x, int y, Color clr )
{
	int yOffset = icon.Tall / 2;
	int xOffset = icon.Wide / 2;
	int xpos = x - xOffset;
	int ypos = y - yOffset;
	vgui2::surface()->DrawSetTexture( icon.Icon );
	vgui2::surface()->DrawSetColor( clr );
	vgui2::surface()->DrawTexturedRect( xpos, ypos, xpos + icon.Wide, ypos + icon.Tall );
}

void CHudAmmo::DrawBackgroundSlot( const Color &clr, int x, int y, CHud::RegisteredIcon icon )
{
	gEngfuncs.pfnFillRGBA( x, y, icon.Wide, icon.Tall, clr.r(), clr.g(), clr.b(), 100 );
}

//
// WeaponList -- Tells the hud about a new weapon type.
//
int CHudAmmo::MsgFunc_WeaponList(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	WEAPON Weapon;

	V_strcpy_safe(Weapon.szName, READ_STRING());
	Weapon.iAmmoType = (int)READ_CHAR();
	Weapon.iMax1 = GetAmmoByAmmoID( Weapon.iAmmoType ).MaxCarry;

	Weapon.iAmmo2Type = READ_CHAR();
	Weapon.iMax2 = GetAmmoByAmmoID( Weapon.iAmmo2Type ).MaxCarry;

	Weapon.iSlot = READ_CHAR();
	Weapon.iSlotPos = READ_CHAR();
	Weapon.iId = READ_CHAR();
	Weapon.iFlags = READ_BYTE();
	Weapon.iWeight = READ_BYTE();
	Weapon.iClip = 0;
	Weapon.iMaxClip = 0;

	if (Weapon.iId < 0 || Weapon.iId >= MAX_WEAPONS)
		return 0;

	if (Weapon.iSlot < 0 || Weapon.iSlot >= MAX_WEAPON_SLOTS + 1)
		return 0;

	if (Weapon.iSlotPos < 0 || Weapon.iSlotPos >= MAX_WEAPON_POSITIONS + 1)
		return 0;

	if (Weapon.iAmmoType < -1 || Weapon.iAmmoType >= ZPAmmoTypes::AMMO_MAX)
		return 0;

	if (Weapon.iAmmo2Type < -1 || Weapon.iAmmo2Type >= ZPAmmoTypes::AMMO_MAX)
		return 0;

	if (Weapon.iAmmoType >= 0 && Weapon.iMax1 == 0)
		return 0;

	if (Weapon.iAmmo2Type >= 0 && Weapon.iMax2 == 0)
		return 0;

	gWR.AddWeapon(&Weapon);

	if (Weapon.iSlot > m_iMaxSlot && Weapon.iSlot < MAX_WEAPON_SLOTS)
		m_iMaxSlot = Weapon.iSlot;

	return 1;
}

//------------------------------------------------------------------------
// Command Handlers
//------------------------------------------------------------------------
// Slot button pressed
void CHudAmmo::SlotInput(int iSlot)
{
	// Let the Viewport use it first, for menus
	if (g_pViewport && g_pViewport->SlotInput(iSlot))
		return;

	gWR.SelectSlot(iSlot, FALSE, 1);
}

void CHudAmmo::UserCmd_Slot1(void)
{
	SlotInput(0);
}

void CHudAmmo::UserCmd_Slot2(void)
{
	SlotInput(1);
}

void CHudAmmo::UserCmd_Slot3(void)
{
	SlotInput(2);
}

void CHudAmmo::UserCmd_Slot4(void)
{
	SlotInput(3);
}

void CHudAmmo::UserCmd_Slot5(void)
{
	SlotInput(4);
}

void CHudAmmo::UserCmd_Slot6(void)
{
	SlotInput(5);
}

void CHudAmmo::UserCmd_Slot7(void)
{
	SlotInput(6);
}

void CHudAmmo::UserCmd_Slot8(void)
{
	SlotInput(7);
}

void CHudAmmo::UserCmd_Slot9(void)
{
	SlotInput(8);
}

void CHudAmmo::UserCmd_Slot10(void)
{
	SlotInput(9);
}

void CHudAmmo::UserCmd_Close(void)
{
	if (gpActiveSel)
	{
		gpLastSel = gpActiveSel;
		gpActiveSel = NULL;
		PlaySound("common/wpn_hudoff.wav", 1);
	}
	else
		gEngfuncs.pfnClientCmd("escape");
}

// Selects the next item in the weapon menu
void CHudAmmo::UserCmd_NextWeapon(void)
{
	if (gHUD.m_fPlayerDead || (gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)))
		return;

	if (!gpActiveSel || gpActiveSel == (WEAPON *)1)
		gpActiveSel = m_pWeapon;

	int pos = 0;
	int slot = 0;
	if (gpActiveSel)
	{
		pos = gpActiveSel->iSlotPos + 1;
		slot = gpActiveSel->iSlot;
	}

	for (int loop = 0; loop <= 1; loop++)
	{
		for (; slot < MAX_WEAPON_SLOTS; slot++)
		{
			for (; pos < MAX_WEAPON_POSITIONS; pos++)
			{
				WEAPON *wsp = gWR.GetWeaponSlot(slot, pos);

				if (wsp && gWR.HasAmmo(wsp))
				{
					gpActiveSel = wsp;
					return;
				}
			}

			pos = 0;
		}

		slot = 0; // start looking from the first slot again
	}

	gpActiveSel = NULL;
}

// Selects the previous item in the menu
void CHudAmmo::UserCmd_PrevWeapon(void)
{
	if (gHUD.m_fPlayerDead || (gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)))
		return;

	if (!gpActiveSel || gpActiveSel == (WEAPON *)1)
		gpActiveSel = m_pWeapon;

	int pos = MAX_WEAPON_POSITIONS - 1;
	int slot = MAX_WEAPON_SLOTS - 1;
	if (gpActiveSel)
	{
		pos = gpActiveSel->iSlotPos - 1;
		slot = gpActiveSel->iSlot;
	}

	for (int loop = 0; loop <= 1; loop++)
	{
		for (; slot >= 0; slot--)
		{
			for (; pos >= 0; pos--)
			{
				WEAPON *wsp = gWR.GetWeaponSlot(slot, pos);

				if (wsp && gWR.HasAmmo(wsp))
				{
					gpActiveSel = wsp;
					return;
				}
			}

			pos = MAX_WEAPON_POSITIONS - 1;
		}

		slot = MAX_WEAPON_SLOTS - 1;
	}

	gpActiveSel = NULL;
}

//-------------------------------------------------------------------------
// Drawing code
//-------------------------------------------------------------------------

void CHudAmmo::Draw(float flTime)
{
	int x, y, r, g, b;
	float a;
	int AmmoWidth;

	if (!(gHUD.m_iWeaponBits & (1 << (WEAPON_SUIT))))
		return;

	if ((gHUD.m_iHideHUDDisplay & (HIDEHUD_WEAPONS | HIDEHUD_ALL)))
		return;

	// Draw Weapon Menu
	DrawWList(flTime);

	// Draw our crosshair
	DrawCrosshair();

	if (!(m_iFlags & HUD_ACTIVE))
		return;

	if (!m_pWeapon)
		return;

	WEAPON *pw = m_pWeapon; // shorthand

	AmmoWidth = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;

	if (!hud_dim.GetBool())
	{
		a = MIN_ALPHA + ALPHA_AMMO_MAX;
	}
	else if (m_fFade > 0)
	{
		// Fade the ammo number back to dim
		m_fFade -= (gHUD.m_flTimeDelta * 20);
		if (m_fFade <= 0)
			m_fFade = 0;
		a = MIN_ALPHA + (m_fFade / FADE_TIME) * ALPHA_AMMO_FLASH;
	}
	else
	{
		a = MIN_ALPHA;
	}

	float alphaDim = a;

	a *= gHUD.GetHudTransparency();
	gHUD.GetHudColor(HudPart::Common, 0, r, g, b);
	ScaleColors(r, g, b, a);

	// Draw weapon sprite
	if ( hud_weapon.GetBool() && IsIconValid( m_pWeapon->hInactive ) )
	{
		y = ScreenHeight - m_pWeapon->hInactive.Tall;
		x = ScreenWidth - (8.5 * AmmoWidth) - m_pWeapon->hInactive.Wide;
		vgui2::surface()->DrawSetTexture( m_pWeapon->hInactive.Icon );
		vgui2::surface()->DrawSetColor( Color( r, g, b, a ) );
		vgui2::surface()->DrawTexturedRect( x, y, x + m_pWeapon->hInactive.Wide, y + m_pWeapon->hInactive.Tall );
	}

	// SPR_Draw Ammo
	if ((pw->iAmmoType < 0) && (pw->iAmmo2Type < 0))
		return;

	int iFlags = DHN_DRAWZERO; // draw 0 values

	// Does this weapon have a clip?
	y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 2;
	y += (int)(gHUD.m_iFontHeight * 0.2f);

	// Does weapon have any ammo at all?
	if (m_pWeapon->iAmmoType > 0)
	{
		int iIconWidth = m_pWeapon->hAmmo.Wide;

		if (pw->iClip >= 0)
		{
			a = alphaDim * gHUD.GetHudTransparency();
			gHUD.GetHudAmmoColor(pw->iClip, pw->iMaxClip, r, g, b);
			ScaleColors(r, g, b, a);

			// room for the number and the '|' and the current ammo
			x = ScreenWidth - (8 * AmmoWidth) - iIconWidth;
			x = gHUD.DrawHudNumber(x, y, iFlags | DHN_3DIGITS, pw->iClip, r, g, b);

			wrect_t rc;
			rc.top = 0;
			rc.left = 0;
			rc.right = AmmoWidth;
			rc.bottom = 100;

			int iBarWidth = AmmoWidth / 10;

			x += AmmoWidth / 2;

			// draw the | bar
			FillRGBA(x, y, iBarWidth, gHUD.m_iFontHeight, r, g, b, a);

			x += iBarWidth + AmmoWidth / 2;
			;

			// GL Seems to need this
			x = gHUD.DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gWR.CountAmmo(pw->iAmmoType), r, g, b);
		}
		else
		{
			a = alphaDim * gHUD.GetHudTransparency();
			gHUD.GetHudAmmoColor(gWR.CountAmmo(pw->iAmmoType), pw->iMax1, r, g, b);
			ScaleColors(r, g, b, a);

			// SPR_Draw a bullets only line
			x = ScreenWidth - 4 * AmmoWidth - iIconWidth;
			x = gHUD.DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gWR.CountAmmo(pw->iAmmoType), r, g, b);
		}

		// Draw the ammo Icon
		if ( IsIconValid( m_pWeapon->hAmmo ) )
		{
			int iOffset = m_pWeapon->hAmmo.Wide / 8;
			vgui2::surface()->DrawSetTexture( m_pWeapon->hAmmo.Icon );
			vgui2::surface()->DrawSetColor( Color( r, g, b, a ) );
			vgui2::surface()->DrawTexturedRect( x, y - iOffset, x + m_pWeapon->hAmmo.Wide, y - iOffset + m_pWeapon->hAmmo.Tall );
		}
	}

	// Does weapon have seconday ammo?
	if (pw->iAmmo2Type > 0)
	{
		int iIconWidth = m_pWeapon->hAmmo2.Wide;

		// Do we have secondary ammo?
		if ((pw->iAmmo2Type != 0) && (gWR.CountAmmo(pw->iAmmo2Type) > 0))
		{
			a = alphaDim * gHUD.GetHudTransparency();
			gHUD.GetHudAmmoColor(pw->iClip, pw->iMaxClip, r, g, b);
			ScaleColors(r, g, b, a);

			y -= gHUD.m_iFontHeight + gHUD.m_iFontHeight / 4;
			x = ScreenWidth - 4 * AmmoWidth - iIconWidth;
			x = gHUD.DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gWR.CountAmmo(pw->iAmmo2Type), r, g, b);

			// Draw the ammo Icon
			if ( IsIconValid( m_pWeapon->hAmmo2 ) )
			{
				int iOffset = m_pWeapon->hAmmo2.Wide / 8;
				vgui2::surface()->DrawSetTexture( m_pWeapon->hAmmo2.Icon );
				vgui2::surface()->DrawSetColor( Color( r, g, b, a ) );
				vgui2::surface()->DrawTexturedRect( x, y - iOffset, x + m_pWeapon->hAmmo2.Wide, y - iOffset + m_pWeapon->hAmmo2.Tall );
			}
		}
	}
}

//
// Draws the ammo bar on the hud
//
int DrawBar(int x, int y, int width, int height, float f)
{
	int r, g, b;
	float a;

	if (f < 0)
		f = 0;
	if (f > 1)
		f = 1;

	if (f)
	{
		int w = f * width;

		// Always show at least one pixel if we have ammo.
		if (w <= 0)
			w = 1;
		a = 255 * gHUD.GetHudTransparency();
		UnpackRGB(r, g, b, RGB_GREENISH);
		FillRGBA(x, y, w, height, r, g, b, a);
		x += w;
		width -= w;
	}

	a = 128 * gHUD.GetHudTransparency();
	gHUD.GetHudColor(HudPart::Common, 0, r, g, b);
	FillRGBA(x, y, width, height, r, g, b, a);

	return (x + width);
}

void DrawAmmoBar(WEAPON *p, int x, int y, int width, int height)
{
	if (!p)
		return;

	if (p->iAmmoType != -1)
	{
		if (!gWR.CountAmmo(p->iAmmoType))
			return;

		float f = (float)gWR.CountAmmo(p->iAmmoType) / (float)p->iMax1;

		x = DrawBar(x, y, width, height, f);

		// Do we have secondary ammo too?

		if (p->iAmmo2Type != -1)
		{
			f = (float)gWR.CountAmmo(p->iAmmo2Type) / (float)p->iMax2;

			x += 5; //!!!

			DrawBar(x, y, width, height, f);
		}
	}
}

//
// Draw Weapon Menu
//
int CHudAmmo::DrawWList(float flTime)
{
	int r, g, b, x, y, i;
	float a;

	if (!gpActiveSel)
		return 0;

	int iActiveSlot;

	if (gpActiveSel == (WEAPON *)1)
		iActiveSlot = -1; // current slot has no weapons
	else
		iActiveSlot = gpActiveSel->iSlot;

	x = SET_WEAPON_SELECTION_XPOS;
	y = SET_WEAPON_SELECTION_YPOS;

	// Ensure that there are available choices in the active slot
	if (iActiveSlot > 0)
	{
		if (!gWR.GetFirstPos(iActiveSlot))
		{
			gpActiveSel = (WEAPON *)1;
			iActiveSlot = -1;
		}
	}

	// Draw top line
	for (i = 0; i <= m_iMaxSlot; i++)
	{
		int iWidth;

		if (iActiveSlot == i)
			a = 255;
		else
			a = 192;

		a *= gHUD.GetHudTransparency();
		gHUD.GetHudColor(HudPart::Common, 0, r, g, b);
		ScaleColors(r, g, b, a);

		SPR_Set(gHUD.GetSprite(m_HUD_bucket0 + i), r, g, b);

		// make active slot wide enough to accomodate gun pictures
		if (i == iActiveSlot)
		{
			WEAPON *p = gWR.GetFirstPos(iActiveSlot);
			if (p)
				iWidth = p->hActive.Wide;
			else
				iWidth = giBucketWidth;
		}
		else
			iWidth = giBucketWidth;

		SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_HUD_bucket0 + i));

		x += iWidth + 5;
	}

	x = SET_WEAPON_SELECTION_XPOS;

	// Draw all of the buckets
	for (i = 0; i <= m_iMaxSlot; i++)
	{
		y = giBucketHeight + SET_WEAPON_SELECTION_YPOS;

		// If this is the active slot, draw the bigger pictures,
		// otherwise just draw boxes
		if (i == iActiveSlot)
		{
			WEAPON *p = gWR.GetFirstPos(i);
			int iWidth = giBucketWidth;
			if (p)
				iWidth = p->hActive.Wide;

			V_HSPRITE hSprSelection = gHUD.GetSprite(m_HUD_selection);
			const wrect_t &rcSprSelection = gHUD.GetSpriteRect(m_HUD_selection);

			for (int iPos = 0; iPos < MAX_WEAPON_POSITIONS; iPos++)
			{
				p = gWR.GetWeaponSlot(i, iPos);

				if (!p || !p->iId)
					continue;

				gHUD.GetHudColor(HudPart::Common, 0, r, g, b);

				// if active, then we must have ammo.

				if (gpActiveSel == p)
				{
					a = 255 * gHUD.GetHudTransparency();
					ScaleColors(r, g, b, a);

					if ( IsIconValid( p->hActive ) )
					{
						Color ItemBG( r, g, b, a );
						DrawBackgroundSlot( ItemBG, x, y, p->hActive );
						vgui2::surface()->DrawSetTexture( p->hActive.Icon );
						vgui2::surface()->DrawSetColor( ItemBG );
						vgui2::surface()->DrawTexturedRect( x, y, x + p->hActive.Wide, y + p->hActive.Tall );
					}
					else
					{
						// Pink rect
						gEngfuncs.pfnFillRGBA(x, y, rcSprSelection.GetWidth(), rcSprSelection.GetHeight(), 64, 0, 64, a);
					}

					SPR_Set(hSprSelection, r, g, b);
					SPR_DrawAdditive(0, x, y, &rcSprSelection);
				}
				else
				{
#if 0
					// Draw Weapon if Red if no ammo

					if (gWR.HasAmmo(p))
					{
						a = 192 * gHUD.GetHudTransparency();
						ScaleColors(r, g, b, a);
					}
					else
					{
						a = 128 * gHUD.GetHudTransparency();
						UnpackRGB(r, g, b, RGB_REDISH);
						ScaleColors(r, g, b, a);
					}
#else
					// Zombie Panic! does not care if you have an empty
					// weapon or not.
					a = 192 * gHUD.GetHudTransparency();
					ScaleColors(r, g, b, a);
#endif

					if ( IsIconValid( p->hInactive ) )
					{
						Color ItemBG( r, g, b, a );
						DrawBackgroundSlot( ItemBG, x, y, p->hInactive );
						vgui2::surface()->DrawSetTexture( p->hInactive.Icon );
						vgui2::surface()->DrawSetColor( ItemBG );
						vgui2::surface()->DrawTexturedRect( x, y, x + p->hInactive.Wide, y + p->hInactive.Tall );
					}
					else
					{
						// Pink rect
						gEngfuncs.pfnFillRGBA(x, y, rcSprSelection.GetWidth(), rcSprSelection.GetHeight(), 48, 0, 48, a);
					}
				}

				// We use the ammobank for showing amount of ammo.
				// And also our weight.
#if 0
				// Draw Ammo Bar
				DrawAmmoBar(p, x + giABWidth / 2, y, giABWidth, giABHeight);
#endif

				y += rcSprSelection.bottom - rcSprSelection.top + 5;
			}

			x += iWidth + 5;
		}
		else
		{
			// Draw Row of weapons.

			for (int iPos = 0; iPos < MAX_WEAPON_POSITIONS; iPos++)
			{
				WEAPON *p = gWR.GetWeaponSlot(i, iPos);

				if (!p || !p->iId)
					continue;

				if (gWR.HasAmmo(p))
				{
					a = 128 * gHUD.GetHudTransparency();
					gHUD.GetHudColor(HudPart::Common, 0, r, g, b);
				}
				else
				{
					a = 96 * gHUD.GetHudTransparency();
					UnpackRGB(r, g, b, RGB_REDISH);
				}

				FillRGBA(x, y, giBucketWidth, giBucketHeight, r, g, b, a);

				y += giBucketHeight + 5;
			}

			x += giBucketWidth + 5;
		}
	}

	return 1;
}

/* =================================
	GetSpriteFromList

Finds and returns the sprite which name starts
with 'pszNameStart' and resolution 'iRes'
in the given sprite list 'pList'
iCount is the number of items in the pList
================================= */
client_sprite_t *GetSpriteFromList(client_sprite_t *pList, const char *pszNameStart, int iRes, int iCount)
{
	if (!pList || iCount <= 0)
		return nullptr;

	int len = strlen(pszNameStart);
	client_sprite_t *p = pList;
	client_sprite_t *pFallback = nullptr; //!< Fallback sprite if can't find for current HUD scale
	while (iCount--)
	{
		if (!strncmp(pszNameStart, p->szName, len))
		{
			if (p->iRes == iRes)
			{
				// Found the requested sprite
				return p;
			}
			else if (p->iRes == HUD_FALLBACK_RES)
			{
				// At least found a fallbcak
				pFallback = p;
			}
		}

		p++;
	}

	// Return the fallback. It may be null but that's fine.
	return pFallback;
}
