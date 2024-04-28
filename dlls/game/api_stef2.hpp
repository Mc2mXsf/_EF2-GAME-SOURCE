//--------------------------------------------------------------
// GAMEFIX - ELITE FORCE 2 - API
// 
// Elite Force 2 Specific Functions and classes are made available
// here in a fashion that the regular gamefix code can be ported
// to fakk2 and others, with very little effort
//--------------------------------------------------------------


#pragma once
#include "_pch_cpp.h"

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


bool gameFix_inSingleplayer();
bool gameFix_inMultiplayer();
bool gameFix_isSpectator_stef2(Entity* ent);
bool gameFix_isDead(Entity* ent);
bool gameFix_isDedicatedServer();
bool gameFix_isListenServer();
bool gameFix_isWindowsServer();
bool gameFix_isLinuxServer();
void gameFix_hudPrint(Player* player, str sText);
int gameFix_maxClients();
bool gameFix_isBot(gentity_t* ent);
bool gameFix_isBot(Player* player);


void gameFix_clearArchetypeInfoDisplay(Player* player, Entity* entity);
Entity* gameFix_getTargetedEntity(Player* player);
Player* gameFix_getClosestPlayerInCallvolume(Entity* entity);
qboolean gameFix_languageEng(const gentity_t* ent);
qboolean gameFix_languageDeu(const gentity_t* ent);
str gameFix_getServerLanguage();
str gameFix_getLanguage(Player* player);
Entity* gameFix_getActorFollowTargetEntity(Actor* actor);