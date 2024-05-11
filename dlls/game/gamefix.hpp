//--------------------------------------------------------------
// GAMEFIX - GENERAL CODE USED FOR GAME-FIXES
// 
// Elite Force 2 Specific Functions and classes go to: api_stef2.cpp
//--------------------------------------------------------------


#pragma once
#include "_pch_cpp.h"
#include "api_stef2.hpp"

//WATER DROWN AND DAMAGE VALUES
constexpr auto _GFix_PLAYER_air_finished = 20.0f; //was 20
constexpr auto _GFix_PLAYER_drown_damage = 2.0f; //was 2.0f
constexpr auto _GFix_PLAYER_drown_damage_max = 15.0f; //was 15.0f
constexpr auto _GFix_PLAYER_increase = 1.0f; //was 2.0f
constexpr auto _GFix_PLAYER_next_drown_time_delay = 3.0f; //was 2.0f
constexpr auto _GFix_PLAYER_next_painsound_time = 3.0f; //was 3.0f

//--------------------------------------------------------------
// GAMEFIX - Added: Information we want to persist over level changes and restarts - chrissstrahl
//--------------------------------------------------------------
struct gamefix_client_persistant_s
{
	int				entNum = -1;
	bool			isBot = false;
	str				language = "Eng";
	bool			admin = false;
};
extern gamefix_client_persistant_s gamefix_client_persistant_t[MAX_CLIENTS];

//--------------------------------------------------------------
// GAMEFIX - Added:  - chrissstrahl
//--------------------------------------------------------------
struct gamefix_entity_extraData_s
{
	EntityPtr		activator = nullptr;
	float			lastActivated = -9999.0f;
};
extern gamefix_entity_extraData_s gamefix_entity_extraData_t[MAX_GENTITIES];

Entity*				gamefix_returnInfoPlayerStart(str info);
Entity*				gamefix_getEntity(str& name);
Player*				gamefix_getPlayer(int index);
int					gamefix_getPlayers(bool state);
Player*				gamefix_getPlayerInsideOfEntity(Entity* eTheBox);
bool				gamefix_checkEntityInsideOfEntity(Entity* eCheck, Entity* eTheBox);
bool				gamefix_targetedByOtherPlayer(Player* player, Entity* entity);
Player*				gamefix_getActivatorOrClosestPlayer(Entity* entity);
Player*				gamefix_getActivatorOrClosestPlayerCansee(Actor* actor);
Player*				gamefix_getClosestPlayer(Entity* entity);
Player*				gamefix_getClosestPlayerSamePlane(Entity* entity);
Player*				gamefix_getClosestPlayer(Entity* entity, bool noSpectator, bool noDead, bool samePlane, int planeMaxVertDiff, int planeMaxRange);
Player*				gamefix_getClosestPlayerToFollow(Actor* actor);
Player*				gamefix_getClosestPlayerActorCanSee(Actor* actor, qboolean useFOV);
Player*				gamefix_getClosestPlayerCanseeIfNoCurEnemy(Actor* actor);
Player*				gamefix_getAnyPlayerPreferably();
Player*				gamefix_getAnyPlayerPreferably(bool noDead, bool noSpectator);
int					gamefix_getCvarInt(str cvarName);
float				gamefix_getCvarFloat(str cvarName);
str					gamefix_getCvar(str cvarName);
bool				gamefix_EntityValid(Entity* entity);
bool				gamefix_PlayerValid();
bool				gamefix_checkNotarget(Entity* entity);
Entity*				gamefix_getActorFollowTarget(Actor* actor);
bool				gamefix_actorCanSee(Actor* actor, Entity* entity, qboolean useFOV, qboolean useVisionDistance);
Entity*				gamefix_actorGetCurrentEnemy(Actor* actor);
bool				gamefix_actorHates(Actor* actor, Entity* entity);
bool				gamefix_checkPlayerRanged(Actor* actor, Player* player);
bool				gamefix_checkPlayerRanged(Actor* actor, bool closestOnly);
bool				gamefix_checkPlayerUsingWeaponNamed(Player* player, const str& weaponNameOfPlayer);
Player*				gamefix_getPlayerByTargetname(const str& targetname);
str					gamefix_getServerLanguage();
str					gamefix_getLanguage(Player* player);
qboolean			gamefix_languageEng(const gentity_t* ent);
qboolean			gamefix_languageDeu(const gentity_t* ent);
void				gamefix_vstrLocalLanguage(gentity_t* ent);
str					gamefix_getLocalizedString(Player* player, const str sEnglish,const str sGerman);
void				gamefix_aiTurnOff();
void				gamefix_aiTurnOn();
void				gamefix_printAllClients(const str text);
void				gamefix_levelFixes();
void				gamefix_playerSpectator(Player* player);
void				gamefix_playerKilled(Player* player);
void				gamefix_playerEntered(Player* player);
void				gamefix_playerSpawn(Player* player);
void				gamefix_playerModelChanged(Player* player);
void				gamefix_dialogSetupPlayers(Actor* speaker, char localizedDialogName[MAX_QPATH], bool headDisplay);
str					gamefix_localizeStringForPlayer(Player* player, char unlocal[MAX_QPATH]);
float				gamefix_dialogGetSoundlength(char sound[MAX_QPATH]);
void				gamefix_replaceSubstring(char* str, const char* find, const char* replace);