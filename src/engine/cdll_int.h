/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
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
//  cdll_int.h
//
// 4-23-98
// JOHN:  client dll interface declarations
//

#ifndef CDLL_INT_H
#define CDLL_INT_H

#include "const.h"
#include "steam/steamtypes.h"
#include "ref_params.h"
#include "r_efx.h"
#include "studio_event.h"
#include "in_buttons.h"

// This cannot be HSPRITE!!
// One cpp file has winsock.h, which will break shit.
typedef int V_HSPRITE; // handle to a graphic

#define SCRINFO_SCREENFLASH 1
#define SCRINFO_STRETCHED   2

struct SCREENINFO
{
	int iSize;
	int iWidth;
	int iHeight;
	int iFlags;
	int iCharHeight;
	short charWidths[256];
};

struct client_data_t
{
	// fields that cannot be modified  (ie. have no effect if changed)
	Vector origin;

	// fields that can be changed by the cldll
	Vector viewangles;
	int iWeaponBits;
	//	int		iAccessoryBits;
	float fov; // field of view
};

struct client_sprite_t
{
	char szName[64];
	char szSprite[64];
	int hspr;
	int iRes;
	wrect_t rc;
};

struct hud_player_info_t
{
	char *name;
	short ping;
	byte thisplayer; // TRUE if this is the calling player

	byte spectator;
	byte packetloss;

	char *model;
	short topcolor;
	short bottomcolor;

	uint64 m_nSteamID;
};

#define CLDLL_INTERFACE_VERSION 7
#include "APIProxy.h"

#endif
