//--------------------------------------------------------------
// GAMEFIX - GENERAL CODE USED FOR GAME-FIXES
// 
// Elite Force 2 Specific Functions and classes go to: api_stef2.cpp
//--------------------------------------------------------------


#include "gamefix.hpp"
#include "api_stef2.hpp"


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
Player* gameFix_getPlayer(int index)
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

	if (gamefix_isSpectator_stef2(eCheck) || gamefix_isSpectator_stef2(eTheBox)) {
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
			if (!ent || !ent->inuse || !ent->client || !ent->entity || player->entnum == i || gameFix_isDead(ent->entity) || gamefix_isSpectator_stef2(ent->entity)) {
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
Player* gameFix_getClosestPlayer(Entity* entity)
{
	return gameFix_getClosestPlayer(entity, true, true);
}

Player* gameFix_getClosestPlayer(Entity* entity,bool noSpectator, bool noDead)
{
	if (gameFix_inSingleplayer()) {
		return gameFix_getPlayer(0);
	}

	if (!entity) {
		return nullptr;
	}

	Player* playerClosest = nullptr;
	float distanceClosest = 999999;

	Player* player = nullptr;
	for (int i = 0; i < gameFix_maxClients(); i++) {
		player = gameFix_getPlayer(i);

		if (!player) {
			continue;
		}

		if (noDead && gameFix_isDead((Entity*)player) || noSpectator && gamefix_isSpectator_stef2((Entity*)player)) {
			continue;
		}

		float distanceCurrent = VectorLength(player->centroid - entity->centroid);
		if (distanceClosest > distanceCurrent) {
			distanceClosest = distanceCurrent;
			playerClosest = player;
		}
	}
	return player;
}