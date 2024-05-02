//--------------------------------------------------------------
// GAMEFIX - ELITE FORCE 2 - API
// 
// Elite Force 2 Specific Functions and classes are made available
// here in a fashion that the regular gamefix code can be ported
// to fakk2 and others, with very little effort
//--------------------------------------------------------------


#pragma once
#include "_pch_cpp.h"
#include "api_stef2_strings.hpp"
#include "gamefix_strings.hpp"
#include "gamefix.hpp"


bool gameFixAPI_inSingleplayer();
bool gameFixAPI_inMultiplayer();
bool gameFixAPI_isSpectator_stef2(Entity* ent);
bool gameFixAPI_isDead(Entity* ent);
bool gameFixAPI_isDedicatedServer();
bool gameFixAPI_isListenServer();
bool gameFixAPI_isWindowsServer();
bool gameFixAPI_isLinuxServer();
void gameFixAPI_hudPrint(Player* player, str sText);
int gameFixAPI_maxClients();
bool gameFixAPI_isBot(gentity_t* ent);
bool gameFixAPI_isBot(Player* player);


void gameFixAPI_clearArchetypeInfoDisplay(Player* player, Entity* entity);
Entity* gameFixAPI_getTargetedEntity(Player* player);
Player* gameFixAPI_getClosestPlayerInCallvolume(Entity* entity);
qboolean gameFixAPI_languageEng(const gentity_t* ent);
qboolean gameFixAPI_languageDeu(const gentity_t* ent);
str gameFixAPI_getServerLanguage();
str gameFixAPI_getLanguage(Player* player);
Entity* gameFixAPI_getActorFollowTargetEntity(Actor* actor);
bool gameFixAPI_actorCanSee(Actor* actor, Entity* entity, bool useFOV, bool useVisionDistance);
Entity* gameFixAPI_actorGetCurrentEnemy(Actor* actor);
bool gameFixAPI_actorHates(Actor* actor,Sentient *sentient);
bool gameFixAPI_checkPlayerUsingWeaponNamed(Player* player, const str& weaponNameOfPlayer);
int gameFixAPI_getPlayers(bool state);
void gameFixAPI_hudPrintAllClients(const str text);