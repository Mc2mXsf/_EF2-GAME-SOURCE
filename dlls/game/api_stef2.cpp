//--------------------------------------------------------------
// GAMEFIX - ELITE FORCE 2 - API
// 
// Elite Force 2 Specific Functions and classes are made available
// here in a fashion that the regular gamefix code can be ported
// to fakk2 and others, with very little effort
//--------------------------------------------------------------


#include "api_stef2.hpp"
#include "gamefix.hpp"
#include "mp_manager.hpp"


//--------------------------------------------------------------
// GAMEFIX - Returns if we are in Singleplayer - chrissstrahl
//--------------------------------------------------------------
bool gameFix_inSingleplayer()
{
	return !gameFix_inMultiplayer();
}

//--------------------------------------------------------------
// GAMEFIX - Returns if we are in Multiplayer - chrissstrahl
//--------------------------------------------------------------
bool gameFix_inMultiplayer()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		return false;
	}
	if (!multiplayerManager.inMultiplayer()) {
		return false;
	}
	return true;
	//FAKK2 Equivalent is probably just a check for deathmatch->integer == 0
}

//--------------------------------------------------------------
// GAMEFIX - Returns if given entity(Player) is a spectator - chrissstrahl
//--------------------------------------------------------------
bool gameFix_isSpectator_stef2(Entity* ent)
{
	if (!ent || !gameFix_inMultiplayer()) {
		return false;
	}

	if ( ent->isSubclassOf(Player) && multiplayerManager.isPlayerSpectator((Player*)ent)) {
		return true;
	}
	return false;
	//FAKK2 Equivalent does not exist, there is just PM_NOCLIP
}

//--------------------------------------------------------------
// GAMEFIX - Returns if given entity(Sentient) is dead - chrissstrahl
//--------------------------------------------------------------
bool gameFix_isDead(Entity* ent)
{
	if (!ent || ent->isSubclassOf(Sentient) && ent->deadflag) {
		return true;
	}
	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Returns if server is dedicated - chrissstrahl
//--------------------------------------------------------------
bool gameFix_isDedicatedServer()
{
	return dedicated->integer;
}

//--------------------------------------------------------------
// GAMEFIX - Returns if this is a Listen Server - chrissstrahl
//--------------------------------------------------------------
bool gameFix_isListenServer()
{
	return !dedicated->integer;
}


//--------------------------------------------------------------
// GAMEFIX - Returns if server is running on Windows - chrissstrahl
//--------------------------------------------------------------
bool gameFix_isWindowsServer()
{
#ifdef WIN32
	return true;
#else
	return false;
#endif
	//FAKK2 Equivalent is _WIN32
}

//--------------------------------------------------------------
// GAMEFIX - Returns if server is running on Linux - chrissstrahl
//--------------------------------------------------------------
bool gameFix_isLinuxServer()
{
#ifdef __linux__
	return true;
#else
	return false;
#endif
}

//--------------------------------------------------------------
// GAMEFIX - Returns if player is host - chrissstrahl
//--------------------------------------------------------------
bool gameFix_isHost(Player *player)
{
#ifdef __linux__
	return false;
#endif

	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER) {
		return true;
	}

	cvar_t* cl_running = gi.cvar_get("cl_running");
	if (dedicated->integer == 0 && player->entnum == 0 && (cl_running ? cl_running->integer : 0)) {
		return true;
	}
	return false;
	//FAKK2 Equivalent might involve: deathmatch->integer + dedicated->integer + cl_running ? + there is no linux in fakk2
}

//--------------------------------------------------------------
// GAMEFIX - Prints Text to player hud/chat - chrissstrahl
//--------------------------------------------------------------
void gameFix_hudPrint(Player* player, str sText)
{
	if (player && sText && sText.length()) {
		player->hudPrint(sText.c_str());
	}
	//FAKK2 Equivalent or the clostest thing to that would be gi.centerprintf and probably gi.SendServerCommand( NULL, "print \"%s\"", text );
}

//--------------------------------------------------------------
// GAMEFIX - Returns current maxclients - chrissstrahl
//--------------------------------------------------------------
int gameFix_maxClients()
{
	return game.maxclients;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to check if player is a BOT - chrissstrahl
//--------------------------------------------------------------
bool gameFix_isBot(Player* player) {
	gentity_t* ent = &g_entities[player->entnum];
	return gameFix_isBot(ent);
}

bool gameFix_isBot(gentity_t* ent)
{
	if (!ent || !ent->client || gameFix_inSingleplayer()) {
		return false;
	}

	if (ent->svflags & SVF_BOT || gamefix_client_persistant_t[ent->s.number].isBot ) {
		return true;
	}

	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Return Entity the Player is currently targeting - chrissstrahl
//--------------------------------------------------------------
Entity* gameFix_getTargetedEntity(Player* player)
{
	if (!player) {
		return nullptr;
	}
	return player->GetTargetedEntity();

	//FAKK2 Equivalent can probably be deduced from Player::AcquireTarget
}

//--------------------------------------------------------------
// GAMEFIX - Returns CallvolumeTrigger Name this player is currently inside of - chrissstrahl
//--------------------------------------------------------------
str gameFix_getCurrentCallVolume(Player* player)
{
	if (!player) {
		return "";
	}
	return player->GetCurrentCallVolume();
	//FAKK2 Equivalent does not exist - trigger_volume_callvolume
}

//--------------------------------------------------------------
// GAMEFIX - Clears Archetype if no other player is targeting the given entity - chrissstrahl
//--------------------------------------------------------------
void gameFix_clearArchetypeInfoDisplay(Player* player, Entity* entity)
{
	if (player && entity) {
		Entity* curTarget = player->GetTargetedEntity();
		if (curTarget && !gamefix_targetedByOtherPlayer(player, curTarget)) {
			curTarget->edict->s.eFlags &= ~(EF_DISPLAY_INFO | EF_DISPLAY_DESC1 | EF_DISPLAY_DESC2 | EF_DISPLAY_DESC3);
		}
	}
	//FAKK2 Equivalent does not exist - EF_DISPLAY_INFO - EF_DISPLAY_DESC1 - EF_DISPLAY_DESC2 - EF_DISPLAY_DESC3
}

//--------------------------------------------------------------
// GAMEFIX - Returns closest player to given entity that also is inside a callvolume-trigger - chrissstrahl
//--------------------------------------------------------------
Player* gameFix_getClosestPlayerInCallvolume(Entity* entity)
{
	if (gameFix_inSingleplayer()) {
		return gamefix_getPlayer(0);
	}

	if (!entity) {
		return nullptr;
	}

	Player* playerClosest = nullptr;
	float distanceClosest = 999999;

	Player* player = nullptr;
	for (int i = 0; i < gameFix_maxClients(); i++) {
		player = gamefix_getPlayer(i);

		if (!player) {
			continue;
		}

		if (gameFix_isDead((Entity*)player) || gameFix_isSpectator_stef2((Entity*)player)) {
			continue;
		}

		if (!strlen(gameFix_getCurrentCallVolume(player))) {
			continue;
		}
		
		float distanceCurrent = VectorLength(player->centroid - entity->centroid);
		if (distanceClosest > distanceCurrent) {
			distanceClosest = distanceCurrent;
			playerClosest = player;
		}
	}
	return playerClosest;
	//FAKK2 Equivalent does not exist - trigger_volume_callvolume
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function that allows Language selection English - chrissstrahl
//--------------------------------------------------------------
qboolean gameFix_languageEng(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	gamefix_client_persistant_t[ent->client->ps.clientNum].language = "Eng";
	
	//after x sec on server assume client typed the command
	if ((player->client->pers.enterTime + 5) < level.time) {	
		gameFix_hudPrint(player,"Your Language was set to English\n");
	}
	return true;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function that allows Language selection German - chrissstrahl
//--------------------------------------------------------------
qboolean gameFix_languageDeu(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	gamefix_client_persistant_t[ent->client->ps.clientNum].language = "Deu";
	
	//after x sec on server assume client typed the command
	if ((player->client->pers.enterTime + 5) < level.time) {
		gameFix_hudPrint(player,"Ihre Sprache wurde auf deutsch festgelegt\n");
	}
	return true;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function retrieving Server Language - chrissstrahl
//--------------------------------------------------------------
str gameFix_getServerLanguage()
{
	if (local_language->string) {
		return local_language->string;
	}
	return "Eng";
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function retrieving Server Language - chrissstrahl
//--------------------------------------------------------------
str gameFix_getLanguage(Player* player)
{
	if (player) {
		if (gameFix_inSingleplayer()) {
			if (local_language->string) {
				return local_language->string;
			}
		}
		else {
			return gamefix_client_persistant_t[player->client->ps.clientNum].language;
		}
	}
	return "Eng";
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function retrieving actor follow target entity - chrissstrahl
//--------------------------------------------------------------
Entity* gameFix_getActorFollowTargetEntity(Actor *actor) {
	if (!actor) { return nullptr; }
	return (Entity*)actor->followTarget.specifiedFollowTarget;
}
