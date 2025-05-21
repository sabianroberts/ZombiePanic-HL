//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#if !defined(IN_DEFSH)
#define IN_DEFSH
#pragma once

// up / down
#define PITCH 0
// left / right
#define YAW 1
// fall over
#define ROLL 2

// ZOMBIE PANIC - START
enum ZPPlayerMovementDirection_t
{
	DirForward,
	DirBack,
	DirSides
};
extern float GetMaxPossibleSpeed( ZPPlayerMovementDirection_t dir );
// ZOMBIE PANIC - STOP

#endif
