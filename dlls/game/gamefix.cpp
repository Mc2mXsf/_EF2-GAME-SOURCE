//--------------------------------------------------------------
// GAMEFIX - GENERAL CODE USED FOR GAME-FIXES
// 
// Elite Force 2 Specific Functions and classes go to: api_stef2.cpp
//--------------------------------------------------------------


#include "gamefix.hpp"
#include "api_stef2.hpp"

//--------------------------------------------------------------
//Added: Information we want to persist over level changes and restarts - See g_local.h for the struct - chrissstrahl
//--------------------------------------------------------------
gamefix_client_persistant_s gamefix_client_persistant_t[MAX_CLIENTS];

//--------------------------------------------------------------
// GAMEFIX - Added: Function to return the singleplayer spawnspot if no multiplayer spawn spot was found - chrissstrahl
//--------------------------------------------------------------
Entity* gamefix_returnInfoPlayerStart()
{
	return G_FindClass(NULL, "info_player_start");
}

//--------------------------------------------------------------
// GAMEFIX - Return Player by client number - chrissstrahl
//--------------------------------------------------------------
Player* gamefix_getPlayer(int index)
{
	gentity_t* ed;

	if (index > gameFix_maxClients())
		return nullptr;

	ed = &g_entities[index];

	if (!ed || !ed->inuse || !ed->entity)
		return nullptr;

	return (Player*)g_entities[index].entity;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to get player that is inside of the given entity boundingbox - chrissstrahl
//--------------------------------------------------------------
Player* gamefix_getPlayerInsideOfEntity(Entity* eTheBox)
{
	Player* player = nullptr;
	Player* playerReturn = nullptr;

	if (!eTheBox || gameFix_isDead(eTheBox) || gameFix_isSpectator_stef2(player)) {
		return nullptr;
	}


	for (int i = 0; i < gameFix_maxClients(); i++) {
		player = gamefix_getPlayer(i);

		if (!player || gameFix_isDead((Entity*)player) || gameFix_isSpectator_stef2(eTheBox)) {
			continue;
		}

		Entity* ePlayer = (Entity*)player;
		if ((ePlayer->absmin[0] > eTheBox->absmax[0]) ||
			(ePlayer->absmin[1] > eTheBox->absmax[1]) ||
			(ePlayer->absmin[2] > eTheBox->absmax[2]) ||
			(ePlayer->absmax[0] < eTheBox->absmin[0]) ||
			(ePlayer->absmax[1] < eTheBox->absmin[1]) ||
			(ePlayer->absmax[2] < eTheBox->absmin[2]))
		{
			continue;
		}

		playerReturn = player;
	}
	return playerReturn;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function used to check if entity is inside boundingbox of other-entity - chrissstrahl
//--------------------------------------------------------------
bool gamefix_checkEntityInsideOfEntity(Entity* eCheck, Entity* eTheBox)
{
	if (!eCheck || !eTheBox || eCheck == eTheBox) {
		return false;
	}

	if (gameFix_isDead(eCheck) || gameFix_isDead(eTheBox)) {
		return false;
	}

	if (gameFix_isSpectator_stef2(eCheck) || gameFix_isSpectator_stef2(eTheBox)) {
		return false;
	}

	if ((eCheck->absmin[0] > eTheBox->absmax[0]) ||
		(eCheck->absmin[1] > eTheBox->absmax[1]) ||
		(eCheck->absmin[2] > eTheBox->absmax[2]) ||
		(eCheck->absmax[0] < eTheBox->absmin[0]) ||
		(eCheck->absmax[1] < eTheBox->absmin[1]) ||
		(eCheck->absmax[2] < eTheBox->absmin[2]))
	{
		return false;
	}
	return true;
}

//--------------------------------------------------------------
// GAMEFIX - Checks if any other player is targeting the given entity - chrissstrahl
//--------------------------------------------------------------
bool gamefix_targetedByOtherPlayer(Player* player, Entity* entity)
{
	if (player && !gameFix_inSingleplayer()) {
		for (int i = 0; i < gameFix_maxClients(); ++i) {
			gentity_t* ent = g_entities + i;
			if (!ent || !ent->inuse || !ent->client || !ent->entity || player->entnum == i || gameFix_isDead(ent->entity) || gameFix_isSpectator_stef2(ent->entity)) {
				continue;
			}

			Entity* curTarget = gameFix_getTargetedEntity(player);
			Player* playerOther = (Player*)ent;
			if (curTarget && curTarget == entity) {
				return true;
			}
		}
	}
	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Returns closest player to given entity - chrissstrahl
//--------------------------------------------------------------
Player* gamefix_getClosestPlayer(Entity* entity)
{
	return gamefix_getClosestPlayer(entity, true, true, false, 0, 0);
}

Player* gamefix_getClosestPlayerSamePlane(Entity* entity)
{
	return gamefix_getClosestPlayer(entity, true, true, true, 196, 1024);
}

Player* gamefix_getClosestPlayer(Entity* entity,bool noSpectator, bool noDead,bool samePlane,int planeMaxVertDiff, int planeMaxRange)
{
	if (gameFix_inSingleplayer()) {
		return gamefix_getPlayer(0);
	}

	if (!entity) {
		return nullptr;
	}

	Player* playerClosest = nullptr;
	float distanceClosest = 999999;

	Player* playerClosestSamePlane = nullptr;
	float distanceClosestSamePlane = 999999;

	Player* player = nullptr;

	for (int i = 0; i < gameFix_maxClients(); i++) {
		player = gamefix_getPlayer(i);

		if (!player) {
			continue;
		}

		if (noDead && gameFix_isDead((Entity*)player) || noSpectator && gameFix_isSpectator_stef2((Entity*)player)) {
			continue;
		}

		float distanceCurrent = VectorLength(player->centroid - entity->centroid);
		
		//always grab player closest as a backup
		if (distanceClosest > distanceCurrent) {
			distanceClosest = distanceCurrent;
			playerClosest = player;
		}

		//have limited range for same plane check
		if (samePlane) {
			Vector zAxiOther = Vector(0, 0, 0);
			Vector zAxiPlayer = Vector(0, 0, 0);
			zAxiOther[2] = entity->origin[2];
			zAxiPlayer[2] = player->origin[2];
			if (distanceCurrent <= planeMaxRange && VectorLength(zAxiOther - zAxiPlayer) < planeMaxVertDiff && distanceClosestSamePlane > distanceCurrent) {
				distanceClosestSamePlane = distanceCurrent;
				playerClosestSamePlane = player;
			}
		}
	}

	//if we don't have player on same plane within range, fallback
	if (!playerClosestSamePlane) {
		playerClosestSamePlane = player;
	}

	return playerClosestSamePlane;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to return interger value from cVar - chrissstrahl
//--------------------------------------------------------------
int gamefix_getCvarInt(str cvarName)
{
	cvar_t* cvar = gi.cvar_get(cvarName.c_str());
	return (cvar ? cvar->integer : 0);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to return float value from cVar - chrissstrahl
//--------------------------------------------------------------
float gamefix_getCvarFloat(str cvarName)
{
	cvar_t* cvar = gi.cvar_get(cvarName.c_str());
	return (cvar ? cvar->value : 0.0f);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to return string value from cVar - chrissstrahl
//--------------------------------------------------------------
str gamefix_getCvar(str cvarName)
{
	cvar_t* cvar = gi.cvar_get(cvarName.c_str());
	return (cvar ? cvar->string : "");
}