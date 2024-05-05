//--------------------------------------------------------------
// GAMEFIX - GENERAL CODE USED FOR GAME-FIXES
// 
// Elite Force 2 Specific Functions and classes go to: api_stef2.cpp
//--------------------------------------------------------------


#include "gamefix.hpp"
#include "api_stef2.hpp"

//--------------------------------------------------------------
//Added: Information we want to persist over level changes and restarts - multiplayer only - See g_local.h for the struct - chrissstrahl
//--------------------------------------------------------------
gamefix_client_persistant_s gamefix_client_persistant_t[MAX_CLIENTS];

//--------------------------------------------------------------
//Added: Extra Information we can attach to entities - multiplayer only - chrissstrahl
//--------------------------------------------------------------
gamefix_entity_extraData_s gamefix_entity_extraData_t[MAX_GENTITIES];

//--------------------------------------------------------------
// GAMEFIX - Added: Function to return the singleplayer spawnspot if no multiplayer spawn spot was found - chrissstrahl
//--------------------------------------------------------------
Entity* gamefix_returnInfoPlayerStart(str info)
{
	gi.Printf(va(_GFix_INFO_FAILSAFE, info.c_str()));
	return G_FindClass(NULL, "info_player_start");
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function grabbing given entity, like in scripts - chrissstrahl
//--------------------------------------------------------------
Entity* gamefix_getEntity(str& name)
{
	return gameFixAPI_getEntity(name);
}

//--------------------------------------------------------------
// GAMEFIX - Return Player by client number - chrissstrahl
//--------------------------------------------------------------
Player* gamefix_getPlayer(int index)
{
	gentity_t* ed;

	if (index > gameFixAPI_maxClients())
		return nullptr;

	ed = &g_entities[index];

	if (!ed || !ed->inuse || !ed->entity)
		return nullptr;

	return (Player*)g_entities[index].entity;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function returning number of player on the server - chrissstrahl
//--------------------------------------------------------------
int gamefix_getPlayers(bool state)
{
	return gameFixAPI_getPlayers(state);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to get player that is inside of the given entity boundingbox - chrissstrahl
//--------------------------------------------------------------
Player* gamefix_getPlayerInsideOfEntity(Entity* eTheBox)
{
	Player* player = nullptr;
	Player* playerReturn = nullptr;

	if (!eTheBox || gameFixAPI_isDead(eTheBox) || gameFixAPI_isSpectator_stef2(player)) {
		return nullptr;
	}


	for (int i = 0; i < gameFixAPI_maxClients(); i++) {
		player = gamefix_getPlayer(i);

		if (!player || gameFixAPI_isDead((Entity*)player) || gameFixAPI_isSpectator_stef2(eTheBox)) {
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

	if (gameFixAPI_isDead(eCheck) || gameFixAPI_isDead(eTheBox)) {
		return false;
	}

	if (gameFixAPI_isSpectator_stef2(eCheck) || gameFixAPI_isSpectator_stef2(eTheBox)) {
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
	if (player && !gameFixAPI_inSingleplayer()) {
		for (int i = 0; i < gameFixAPI_maxClients(); ++i) {
			gentity_t* ent = g_entities + i;
			if (!ent || !ent->inuse || !ent->client || !ent->entity || player->entnum == i || gameFixAPI_isDead(ent->entity) || gameFixAPI_isSpectator_stef2(ent->entity)) {
				continue;
			}

			Entity* curTarget = gameFixAPI_getTargetedEntity(player);
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
//NO SPECTATOR, NO DEAD
{
	return gamefix_getClosestPlayer(entity, true, true, false, 0, 0);
}

Player* gamefix_getClosestPlayerSamePlane(Entity* entity)
//NO SPECTATOR, NO DEAD, SAME PLANE, MAX Z-Difference (to match plane), MAX X-Difference (to match plane)
{
	return gamefix_getClosestPlayer(entity, true, true, true, 196, 1024);
}

Player* gamefix_getClosestPlayer(Entity* entity,bool noSpectator, bool noDead,bool samePlane,int planeMaxVertDiff, int planeMaxRange)
//RETURNS nullptr IF CRITIRA (spec/dead/notarget) DON'T MATCH
//GIVES PLAYER FROM OTHER PLANES (bigger Z-AXIS differences) IF NONE ON THE SAME PLANE BEFORE GIVING UP AND RETURNING nullptr
{
	if (gameFixAPI_inSingleplayer()) {
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

	for (int i = 0; i < gameFixAPI_maxClients(); i++) {
		player = gamefix_getPlayer(i);

		if (!player) {
			continue;
		}

		if (noDead && gameFixAPI_isDead((Entity*)player) || noSpectator && gameFixAPI_isSpectator_stef2((Entity*)player)) {
			continue;
		}

		if (gamefix_checkNotarget((Entity*)player)) {
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

	//if we don't have player on same plane within range, fallback to our backup
	if (!playerClosestSamePlane) {
		playerClosestSamePlane = playerClosest;
	}

	return playerClosestSamePlane;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to get best closest Player to follow - chrissstrahl
// //--------------------------------------------------------------
Player* gamefix_getClosestPlayerToFollow(Actor* actor)
{
	if (!actor) { return nullptr; }
	Entity* ent = nullptr;

	ent = gamefix_getActorFollowTarget(actor);

	if (gamefix_EntityValid(ent) && ent->isSubclassOf(Player)) {
		return (Player*)ent;
	}

	return gamefix_getClosestPlayerSamePlane((Entity*)actor);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function returning closest player that given actor can see - chrissstrahl
//--------------------------------------------------------------
Player* gamefix_getClosestPlayerActorCanSee(Actor* actor, qboolean useFOV)
{
	if (!actor || gameFixAPI_isDead(actor)) {
		return nullptr;
	}

	Player* playerClosest = nullptr;
	float distanceClosest = 999999;

	Player* player = nullptr;

	for (int i = 0; i < gameFixAPI_maxClients(); i++) {
		player = gamefix_getPlayer(i);

		if (!gamefix_EntityValid((Entity*)player) || !gamefix_actorCanSee(actor, (Entity*)player, useFOV, qtrue)) {
			continue;
		}

		float distanceCurrent = VectorLength(player->centroid - actor->centroid);

		//always grab player closest as a backup
		if (distanceClosest > distanceCurrent) {
			distanceClosest = distanceCurrent;
			playerClosest = player;
		}
	}

	return playerClosest;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function returning current enemy if player or the closest player actor cansee - chrissstrahl
//--------------------------------------------------------------
Player* gamefix_getClosestPlayerCanseeIfNoCurEnemy(Actor* actor)
{
	Entity* ent = gamefix_actorGetCurrentEnemy(actor);
	if (gamefix_EntityValid(ent) && ent->isSubclassOf(Player)) {
		return (Player*)ent;
	}
	return gamefix_getClosestPlayerActorCanSee(actor, qfalse);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function that returns any player preferably not dead or spectator - chrissstrahl
//--------------------------------------------------------------
Player* gamefix_getAnyPlayerPreferably()
{
	return gamefix_getAnyPlayerPreferably(true,true);
}

Player* gamefix_getAnyPlayerPreferably(bool noDead,bool noSpectator)
{
	if (gameFixAPI_inSingleplayer()) {
		return gamefix_getPlayer(0);
	}

	Player* player = nullptr;
	Player* playerDead = nullptr;
	Player* playerSpec = nullptr;

	for (int i = 0; i < gameFixAPI_maxClients(); i++) {
		player = gamefix_getPlayer(i);

		if (!player) {
			continue;
		}

		if (gamefix_checkNotarget((Entity*)player)) {
			continue;
		}

		if (noDead && gameFixAPI_isDead((Entity*)player)) {
			if (!playerDead) {
				playerDead = player;
			}
			continue;
		}
		
		if(noSpectator && gameFixAPI_isSpectator_stef2((Entity*)player)) {
			if (!playerSpec) {
				playerSpec = player;
			}
			continue;
		}
		//yay a player is a match
		break;
	}

	//we didn't get the prefered player, now take any avialable
	if (!player) {
		if (!playerDead) {
			player = playerDead;
		}
		else if(!playerSpec){
			player = playerSpec;
		}
	}

	return player;
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

//--------------------------------------------------------------
// GAMEFIX - Added: Function to check quicky if GIVEN entity exists,alive,nospec,targetable - chrissstrahl
//--------------------------------------------------------------
bool gamefix_EntityValid(Entity* entity)
{
	if (!gameFixAPI_isDead(entity) && !gameFixAPI_isSpectator_stef2(entity) && !gamefix_checkNotarget(entity)) {
		return true;
	}
	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to check quicky if ANY player is alive,nospec,targetable - chrissstrahl
//--------------------------------------------------------------
bool gamefix_PlayerValid()
{
	Player* player = nullptr;
	for (int i = 0; i < gameFixAPI_maxClients(); i++) {
		player = gamefix_getPlayer(i);
		if (gamefix_EntityValid(player)) {
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function checking if entity is in notarget - chrissstrahl
//--------------------------------------------------------------
bool gamefix_checkNotarget(Entity* entity) {
	if (!entity) { return false; }
	return (entity->flags & FL_NOTARGET);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function returning actor follow target entity - chrissstrahl
//--------------------------------------------------------------
Entity* gamefix_getActorFollowTarget(Actor* actor) {
	return gameFixAPI_getActorFollowTargetEntity(actor);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function checking if actor can actually see the given entity - chrissstrahl
//--------------------------------------------------------------
bool gamefix_actorCanSee(Actor* actor, Entity* entity, qboolean useFOV, qboolean useVisionDistance)
{
	return gameFixAPI_actorCanSee(actor,entity,useFOV,useVisionDistance);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function returning current enemy of actor - chrissstrahl
//--------------------------------------------------------------
Entity* gamefix_actorGetCurrentEnemy(Actor* actor)
{
	return gameFixAPI_actorGetCurrentEnemy(actor);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function checking if actor hates given entity - chrissstrahl
//--------------------------------------------------------------
bool gamefix_actorHates(Actor* actor, Entity* entity)
{
	if (actor && entity && entity->isSubclassOf(Sentient)) {
		return gameFixAPI_actorHates(actor, (Sentient*)entity);
	}
	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function checking if given player uses currently a ranged weapon - chrissstrahl
//--------------------------------------------------------------
bool gamefix_checkPlayerRanged(Actor* actor,Player* player)
{
	if (!actor || !player || !gamefix_EntityValid((Entity*)player)) { return false; }
	return (actor->EntityHasFireType((Entity*)player, FT_BULLET) == qboolean(qtrue) || actor->EntityHasFireType((Entity*)player, FT_PROJECTILE) == qboolean(qtrue));
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function checking if GIVEN player has ranged weapon + valid,cansee - chrissstrahl
//--------------------------------------------------------------
bool gamefix_checkPlayerRangedCanidate(Actor* actor,Player* player)
{
	if (actor && gamefix_EntityValid((Entity*)player) &&
		gamefix_checkPlayerRanged(actor, player) &&
		//within vision distance 360-FOV check if ai could see the given player
		gamefix_actorCanSee(actor, (Entity*)player, qfalse, qtrue))
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function checking if ANY player has ranged weapon + curEnemy,followTarget,valid,cansee - chrissstrahl
//--------------------------------------------------------------
bool gamefix_checkPlayerRanged(Actor* actor, bool closestOnly)
{
	if (!actor) { return false; }

	if (gameFixAPI_inSingleplayer()) {
		return gamefix_checkPlayerRanged(actor,GetPlayer(0));
	}
	
	Player* player = nullptr;
	if (closestOnly) {
		return gamefix_checkPlayerRangedCanidate(actor,gamefix_getClosestPlayerActorCanSee(actor,qfalse));
	}
	
	Entity* ent = gamefix_actorGetCurrentEnemy(actor);
	if (ent && ent->isSubclassOf(Player) && gamefix_checkPlayerRanged(actor,(Player*)ent)) {
		return true;
	}

	//meant for teammates - but this might also applay to enemies, I didn't bother to check
	ent = gamefix_getActorFollowTarget(actor);
	if (ent && ent->isSubclassOf(Player) && gamefix_checkPlayerRanged(actor, (Player*)ent)) {
		return true;
	}
	
	//if all else fails, we get to the expensive part
	for (int i = 0; i < gameFixAPI_maxClients(); i++) {
		player = gamefix_getPlayer(i);
		if (gamefix_checkPlayerRangedCanidate(actor, player)){
			return true;
		}
	}
	
	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function checking if given player uses given weapon - chrissstrahl
//--------------------------------------------------------------
bool gamefix_checkPlayerUsingWeaponNamed(Player* player, const str& weaponNameOfPlayer)
{
	return gameFixAPI_checkPlayerUsingWeaponNamed(player,weaponNameOfPlayer);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function returning player by giventargetname - chrissstrahl
//--------------------------------------------------------------
Player* gamefix_getPlayerByTargetname(const str& targetname)
{
	Player* player = nullptr;
	for (int i = 0; i < gameFixAPI_maxClients(); i++) {
		player = gamefix_getPlayer(i);
		if (player && Q_stricmp(player->targetname.c_str(),targetname.c_str())) {
			return player;
		}
	}
	return nullptr;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function retrieving Server Language - chrissstrahl
//--------------------------------------------------------------
str gamefix_getServerLanguage()
{
	return gameFixAPI_getServerLanguage();
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function retrieving Player Language - chrissstrahl
//--------------------------------------------------------------
str gamefix_getLanguage(Player* player) {
	return gameFixAPI_getLanguage(player);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function that allows Language selection English - chrissstrahl
//--------------------------------------------------------------
qboolean gamefix_languageEng(const gentity_t* ent)
{
	return gameFixAPI_languageEng(ent);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function that allows Language selection German - chrissstrahl
//--------------------------------------------------------------
qboolean gamefix_languageDeu(const gentity_t* ent)
{
	return gameFixAPI_languageDeu(ent);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function returning player by giventargetname - chrissstrahl
//--------------------------------------------------------------
str gamefix_getLocalizedString(Player* player,const str sEnglish,const str sGerman)
{
	if (player) {
		if (gameFixAPI_getLanguage(player) == "Deu") {
			return sGerman;
		}
	}
	return sEnglish;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function making the client send the contents of his local_language cvar as console-command - chrissstrahl
// Expected return values: Eng/Deu need to be catched in gamecmds.cpp -> consolecmd_t G_ConsoleCmds[] add:
// {"Eng",gamefix_languageEng,true},
// {"Deu",gamefix_languageDeu,true},
//--------------------------------------------------------------
void gamefix_vstrLocalLanguage(gentity_t* ent)
{
	if (ent && gameFixAPI_inMultiplayer()) {
		gi.SendServerCommand(ent->client->ps.clientNum, "stufftext \"vstr local_language\n\"");
	}
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to turn OFF LEVEL AI if server empty, prevent issues with ai - chrissstrahl
//--------------------------------------------------------------
void gamefix_aiTurnOff()
{
	if (gameFixAPI_inMultiplayer() && gamefix_getPlayers(true) <= 0) {
		level.ai_on = false;
		gi.Printf(_GFixEF2_INFO_FUNC_ClientDisconnect);
	}
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to turn ON LEVEL AI if server no longer empty - chrissstrahl
//--------------------------------------------------------------
void gamefix_aiTurnOn()
{
	if (gameFixAPI_inMultiplayer() && gamefix_getPlayers(true) <= 0) {
		level.ai_on = true;
		gi.Printf(_GFixEF2_INFO_FUNC_ClientBegin);
	}
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to print text to all players - chrissstrahl
//--------------------------------------------------------------
void gamefix_printAllClients(const str text)
{
	gameFixAPI_hudPrintAllClients(text);
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function manage level related fixes - chrissstrahl
//--------------------------------------------------------------
void gamefix_levelFixes()
{
	gameFixAPI_levelFixes();
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function handling player game event - chrissstrahl
//--------------------------------------------------------------
void gamefix_playerSpectator(Player* player)
{
	gameFixAPI_playerSpectator(player);
}
void gamefix_playerKilled(Player* player)
{
	gameFixAPI_playerKilled(player);
}
void gamefix_playerEntered(Player* player)
{
	gameFixAPI_playerEntered(player);
}
void gamefix_playerSpawn(Player* player)
{
	gameFixAPI_playerSpawn(player);
}
void gamefix_playerModelChanged(Player* player)
{
	gameFixAPI_playerModelChanged(player);
}
