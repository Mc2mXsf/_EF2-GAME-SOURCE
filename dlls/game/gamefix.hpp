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
Player*				gamefix_getAnyPlayerPreferably();
Player*				gamefix_getAnyPlayerPreferably(bool noDead, bool noSpectator);
int					gamefix_getCvarInt(str cvarName);
float				gamefix_getCvarFloat(str cvarName);
str					gamefix_getCvar(str cvarName);
bool				gamefix_EntityValid(Entity* entity);
bool				gamefix_PlayerValid();
bool				gamefix_checkNotarget(Entity* entity);
Player*				gamefix_getClosestPlayerToFollow(Actor* actor);
Entity*				gamefix_getActorFollowTarget(Actor* actor);
bool				gamefix_actorCanSee(Actor* actor, Entity* entity, qboolean useFOV, qboolean useVisionDistance);
Player*				gamefix_getClosestPlayerActorCanSee(Actor* actor, qboolean useFOV);
Player*				gamefix_actorGetPlayerCurEnemyOrClosestCansee(Actor* actor);
bool				gamefix_actorHates(Actor* actor, Entity* entity);
bool				gamefix_checkPlayerRanged(Actor* actor, Player* player);
bool				gamefix_checkPlayerRanged(Actor* actor, bool closestOnly);
bool				gamefix_checkPlayerUsingWeaponNamed(Player* player, const str& weaponNameOfPlayer);
Player*				gamefix_getPlayerByTargetname(const str& targetname);