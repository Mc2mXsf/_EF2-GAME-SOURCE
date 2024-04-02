//--------------------------------------------------------------
// GAMEFIX - ELITE FORCE 2 - API
// 
// Elite Force 2 Specific Functions and classes are made available
// here in a fashion that the regular gamefix code can be ported
// to fakk2 and others, with very little effort
//--------------------------------------------------------------


#pragma once
#include "_pch_cpp.h"


bool gameFix_inMultiplayer();
bool gamefix_isSpectator_stef2(Entity* ent);
bool gameFix_isDead(Entity* ent);
bool gameFix_isDedicatedServer();
bool gameFix_isListenServer();
bool gameFix_isWindowsServer();
bool gameFix_isLinuxServer();
void gameFix_hudPrint(Player* player, str sText);