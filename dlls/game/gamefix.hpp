//--------------------------------------------------------------
// GAMEFIX - GENERAL CODE USED FOR GAME-FIXES
// 
// Elite Force 2 Specific Functions and classes go to: api_stef2.cpp
//--------------------------------------------------------------


#pragma once
#include "_pch_cpp.h"
#include "api_stef2.hpp"


Entity*				gamefix_returnInfoPlayerStart();
Player*				gamefix_getPlayer(int index);
Player*				gamefix_getPlayerInsideOfEntity(Entity* eTheBox);
bool				gamefix_checkEntityInsideOfEntity(Entity* eCheck, Entity* eTheBox);
bool				gamefix_targetedByOtherPlayer(Player* player, Entity* entity);
Player*				gamefix_getClosestPlayer(Entity* entity);
Player*				gamefix_getClosestPlayerSamePlane(Entity* entity);
Player*				gamefix_getClosestPlayer(Entity* entity, bool noSpectator, bool noDead, bool samePlane, int planeMaxVertDiff, int planeMaxRange);
int					gamefix_getCvarInt(str cvarName);
float				gamefix_getCvarFloat(str cvarName);
str					gamefix_getCvar(str cvarName);