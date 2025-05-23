#ifndef GAME_CLIENT_KEYVALUESCOMPAT_H
#define GAME_CLIENT_KEYVALUESCOMPAT_H

#include "interface.h"

/**
*	Initializes the KeyValuesSystem accessor.
*	@return Whether initialization succeeded.
*/
bool KV_InitKeyValuesSystem( CreateInterfaceFn* pFactories, int iNumFactories );
bool KV_InitKeyValuesSystemOnServer();

#endif //GAME_CLIENT_KEYVALUESCOMPAT_H
