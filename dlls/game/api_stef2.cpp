//--------------------------------------------------------------
// GAMEFIX - ELITE FORCE 2 - API
// 
// Elite Force 2 Specific Functions and classes are made available
// here in a fashion that the regular gamefix code can be ported
// to fakk2 and others, with very little effort
//--------------------------------------------------------------


#include "api_stef2.hpp"

#include "mp_manager.hpp"


Container<str> gameFixAPI_maplistContainer;

//--------------------------------------------------------------
// GAMEFIX - Returns if we are in Singleplayer - chrissstrahl
//--------------------------------------------------------------
bool gameFixAPI_inSingleplayer()
{
	return !gameFixAPI_inMultiplayer();
}

//--------------------------------------------------------------
// GAMEFIX - Returns if we are in Multiplayer - chrissstrahl
//--------------------------------------------------------------
bool gameFixAPI_inMultiplayer()
{
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		return false;
	}
	if (!multiplayerManager.inMultiplayer()) {
		return false;
	}
#else //FAKK2 / MOHAA
	if (!deathmatch->integer) {
		return false;
	}
#endif //GAME_STAR_TREK_ELITE_FORCE_2
	return true;
}

//--------------------------------------------------------------
// GAMEFIX - Returns if given entity(Player) is a spectator - chrissstrahl
//--------------------------------------------------------------
bool gameFixAPI_isSpectator_stef2(Entity* ent)
{
	if (!ent || !gameFixAPI_inMultiplayer()) {
		return false;
	}

	if (ent->isSubclassOf(Player)) {
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
		if (multiplayerManager.isPlayerSpectator((Player*)ent)) {
			return true;
		}
#else //MOHAA
#endif //GAME_STAR_TREK_ELITE_FORCE_2
	}
	return false;
	//FAKK2 Equivalent does not exist, there is just PM_NOCLIP
}

//--------------------------------------------------------------
// GAMEFIX - Returns if given entity(Sentient) is dead - chrissstrahl
//--------------------------------------------------------------
bool gameFixAPI_isDead(Entity* ent)
{
	if (!ent || ent->isSubclassOf(Sentient) && ent->deadflag) {
		return true;
	}
	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Returns if server is dedicated - chrissstrahl
//--------------------------------------------------------------
bool gameFixAPI_isDedicatedServer()
{
	return dedicated->integer;
}

//--------------------------------------------------------------
// GAMEFIX - Returns if this is a Listen Server - chrissstrahl
//--------------------------------------------------------------
bool gameFixAPI_isListenServer()
{
	return !dedicated->integer;
}


//--------------------------------------------------------------
// GAMEFIX - Returns if server is running on Windows - chrissstrahl
//--------------------------------------------------------------
bool gameFixAPI_isWindowsServer()
{
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
	#ifdef WIN32
		return true;
	#else
		return false;
	#endif
#else //FAKK2 / MOHAA
	#ifdef _WIN32
		return true;
	#else
		return false;
	#endif
#endif //GAME_STAR_TREK_ELITE_FORCE_2
}

//--------------------------------------------------------------
// GAMEFIX - Returns if server is running on Linux - chrissstrahl
//--------------------------------------------------------------
bool gameFixAPI_isLinuxServer()
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
bool gameFixAPI_isHost(Player *player)
{
#ifdef __linux__
	return false;
#endif

#ifdef GAME_STAR_TREK_ELITE_FORCE_2
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER) {
		return true;
	}
#endif //GAME_STAR_TREK_ELITE_FORCE_2

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
void gameFixAPI_hudPrint(Player* player, str sText)
{
	if (player && sText && sText.length()) {
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
		player->hudPrint(sText.c_str());
#else
		gentity_t* edict = player->entnum;
		gi.centerprintf(edict, sText.c_str());
#endif //GAME_STAR_TREK_ELITE_FORCE_2
	}
}

//--------------------------------------------------------------
// GAMEFIX - Returns current maxclients - chrissstrahl
//--------------------------------------------------------------
int gameFixAPI_maxClients()
{
	return game.maxclients;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to check if player is a BOT - chrissstrahl
//--------------------------------------------------------------
bool gameFixAPI_isBot(Player* player) {
	gentity_t* ent = &g_entities[player->entnum];
	return gameFixAPI_isBot(ent);
}

bool gameFixAPI_isBot(gentity_t* ent)
{
	if (!ent || !ent->client || gameFixAPI_inSingleplayer()) {
		return false;
	}

	if (ent->svflags & SVF_BOT || gamefix_client_persistant_t[ent->s.number].isBot ) {
		return true;
	}

	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function handling player game event - chrissstrahl
//--------------------------------------------------------------
void gameFixAPI_playerEntered(Player* player)
{
	if (gameFixAPI_inSingleplayer()) {
		gameFixAPI_playerSpawn(player);
		return;
	}
	//gi.Printf("gameFixAPI_playerEntered\n");
}
void gameFixAPI_playerSpawn(Player* player)
{
	//gi.Printf("gameFixAPI_playerSpawn\n");
}
void gameFixAPI_playerUseItem(Player* player, const char* name)
{
	//gi.Printf("gameFixAPI_playerUseItem %s\n",name);
}
void gameFixAPI_playerKilled(Player* player)
{
	//gi.Printf("gameFixAPI_playerKilled\n");
}
void gameFixAPI_playerChangeTeam(Player* player,const str &realTeamName)
//multiplayer only
{
	if (gameFixAPI_inSingleplayer()) {
		return;
	}
	
	if (gamefix_client_persistant_t[player->entnum].currentTeam != realTeamName) {
		//gi.Printf("gameFixAPI_playerChangeTeam %s\n", realTeamName.c_str());
		gamefix_client_persistant_t[player->entnum].currentTeam == realTeamName;
	}
}
void gameFixAPI_playerSpectator(Player* player)
//multiplayer only
{
	if (gameFixAPI_inSingleplayer()) {
		return;
	}

	//--------------------------------------------------------------
	// GAMEFIX - Added: Make player view from the current camera during cinematic, when just entering or switching around - chrissstrahl
	//--------------------------------------------------------------
	if (level.cinematic == 1 && multiplayerManager.gamefixEF2_currentCamera) {
		player->SetCamera(multiplayerManager.gamefixEF2_currentCamera, 0);
	}
}
void gameFixAPI_playerModelChanged(Player* player)
//multiplayer only
{
	if (gameFixAPI_inSingleplayer()) {
		return;
	}
	
	if (gamefix_client_persistant_t[player->entnum].currentModel != player->model) {
		//gi.Printf("gameFixAPI_playerModelChanged %s\n", player->model.c_str());
		gamefix_client_persistant_t[player->entnum].currentModel == player->model;
	}
}
void gameFixAPI_playerScore(Player* player)
{
}

//--------------------------------------------------------------
// GAMEFIX - Return Entity the Player is currently targeting - chrissstrahl
//--------------------------------------------------------------
Entity* gameFixAPI_getTargetedEntity(Player* player)
{
	if (!player) {
		return nullptr;
	}
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
	return player->GetTargetedEntity();
#else
	gi.Error(ERR_DROP,"gameFixAPI_getTargetedEntity - NOT IMPLEMETENT, please FIX\n");
#endif //GAME_STAR_TREK_ELITE_FORCE_2

	//FAKK2 Equivalent can probably be deduced from Player::AcquireTarget
}

//--------------------------------------------------------------
// GAMEFIX - Returns CallvolumeTrigger Name this player is currently inside of - chrissstrahl
//--------------------------------------------------------------
str gameFixAPI_getCurrentCallVolume(Player* player)
{
	if (!player) {
		return "";
	}
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
	return player->GetCurrentCallVolume();
#else
	gi.Error(ERR_DROP, "gameFixAPI_getCurrentCallVolume - trigger_volume_callvolume NOT IN GAME\n");
#endif //GAME_STAR_TREK_ELITE_FORCE_2
}

//--------------------------------------------------------------
// GAMEFIX - Clears Archetype if no other player is targeting the given entity - chrissstrahl
//--------------------------------------------------------------
void gameFixAPI_clearArchetypeInfoDisplay(Player* player, Entity* entity)
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
Player* gameFixAPI_getClosestPlayerInCallvolume(Entity* entity)
{
	if (gameFixAPI_inSingleplayer()) {
		return gamefix_getPlayer(0);
	}

	if (!entity) {
		return nullptr;
	}

	Player* playerClosest = nullptr;
	float distanceClosest = 999999;

	Player* player = nullptr;
	for (int i = 0; i < gameFixAPI_maxClients(); i++) {
		player = gamefix_getPlayer(i);

		if (!player) {
			continue;
		}

		if (gameFixAPI_isDead((Entity*)player) || gameFixAPI_isSpectator_stef2((Entity*)player)) {
			continue;
		}

		if (!strlen(gameFixAPI_getCurrentCallVolume(player))) {
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
// GAMEFIX - Added: Handles for Persistant Player data - chrissstrahl
//--------------------------------------------------------------
void gameFixAPI_initPersistant(int clientNum, bool isBot)
{
	if (gameFixAPI_inSingleplayer()) {
		return;
	}

	if (clientNum < 0 || clientNum > MAX_CLIENTS) {
		gi.Error(ERR_DROP, _GFixEF2_ERR_gameFixAPI_initPersistantClNum, clientNum);
		return;
	}

	gamefix_client_persistant_t[clientNum].isBot = isBot;
	gamefix_client_persistant_t[clientNum].language = "Eng";
	gamefix_client_persistant_t[clientNum].admin = false;
	gamefix_client_persistant_t[clientNum].commands = 0;
	gamefix_client_persistant_t[clientNum].commandsLast = -90119.0f;
	gamefix_client_persistant_t[clientNum].chats = 0;
	gamefix_client_persistant_t[clientNum].chatsLast = -90118.0f;
	gamefix_client_persistant_t[clientNum].currentModel = "";
	gamefix_client_persistant_t[clientNum].currentTeam = "none";
}

//--------------------------------------------------------------
// GAMEFIX - Used to keep track of client commands - part of the replacements for sv_floodProtect- chrissstrahl
//--------------------------------------------------------------
bool gamefixAPI_commandsUpdate(int clientNum, const str &cmd)
{
	if (gameFixAPI_inSingleplayer()) {
		return true;
	}

	if (clientNum < 0 || clientNum > MAX_CLIENTS) {
		gi.Error(ERR_DROP, _GFixEF2_ERR_gamefixAPI_commandsUpdateClNum, clientNum);
		return false;
	}

	//allow these commands to always pass
	if (Q_stricmp(cmd,"disconnect") == 0 ||
		Q_stricmp(cmd,"say") == 0 ||
		Q_stricmp(cmd,"tsay") == 0 ||
		Q_stricmp(cmd,"vsay") == 0 ||
		Q_stricmp(cmd,"vosay") == 0 ||
		Q_stricmp(cmd,"vosay_team") == 0 ||
		Q_stricmp(cmd,"vsay_team") == 0 ||
		Q_stricmp(cmd,"say_team") == 0 ||
		cmd == "Eng" ||
		cmd == "Deu")
	{
		return true;
	}

	if (gamefix_client_persistant_t[clientNum].commandsLast < level.time) {
		gamefix_client_persistant_t[clientNum].commandsLast = (level.time + GAMEFIX_API_COMMANDS_CYCLE);
		if (strlen(gamefix_client_persistant_t[clientNum].commandsStalled)) {
			Player* player = GetPlayer(clientNum);
			if (player) {
				gi.Printf("Gamefix Stalled Commands (%s) for %s\n", gamefix_client_persistant_t[clientNum].commandsStalled.c_str(), player->client->pers.netname);
			}
		}
		gamefix_client_persistant_t[clientNum].commandsStalled = "";
		gamefix_client_persistant_t[clientNum].commands = 0;
	}

	if (++gamefix_client_persistant_t[clientNum].commands > GAMEFIX_API_COMMANDS_MAX) {
		if (!strstr(gamefix_client_persistant_t[clientNum].commandsStalled.c_str(),cmd.c_str())) {
			gamefix_client_persistant_t[clientNum].commandsStalled += va(" %s", cmd.c_str());
		}
		return false;
	}
	return true;
}

//--------------------------------------------------------------
// GAMEFIX - Used to keep track of client chat - part of the replacements for sv_floodProtect- chrissstrahl
//--------------------------------------------------------------
bool gamefixAPI_chatUpdate(int clientNum, const str &text)
{
	if (gameFixAPI_inSingleplayer()) {
		return true;
	}

	if (clientNum < 0 || clientNum > MAX_CLIENTS) {
		gi.Error(ERR_DROP, _GFixEF2_ERR_gamefixAPI_chatUpdateClNum, clientNum);
		return false;
	}

	if (gamefix_client_persistant_t[clientNum].chatsLast < level.time) {
		gamefix_client_persistant_t[clientNum].chatsLast = (level.time + GAMEFIX_API_CHAT_CYCLE);
		gamefix_client_persistant_t[clientNum].chats = 0;
	}
	
	gamefix_client_persistant_t[clientNum].chats++;
	
	if (gamefix_client_persistant_t[clientNum].chats > GAMEFIX_API_CHAT_MAX) {
		return false;
	}
	return true;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function that allows Language selection English - chrissstrahl
//--------------------------------------------------------------
qboolean gameFixAPI_languageEng(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	gamefix_client_persistant_t[ent->client->ps.clientNum].language = "Eng";
	
	//after x sec on server assume client typed the command
	if ((player->client->pers.enterTime + 5) < level.time) {	
		gameFixAPI_hudPrint(player, _GFixAPI_YOUR_LANG_WAS_SET_TO_ENG);
	}
	return true;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function that allows Language selection German - chrissstrahl
//--------------------------------------------------------------
qboolean gameFixAPI_languageDeu(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	gamefix_client_persistant_t[ent->client->ps.clientNum].language = "Deu";
	
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
	//after x sec on server assume client typed the command
	if (player->client && (player->client->pers.enterTime + 5) < level.time) {
		gameFixAPI_hudPrint(player, _GFixAPI_YOUR_LANG_WAS_SET_TO_DEU);
	}
#endif //GAME_STAR_TREK_ELITE_FORCE_2

	return true;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function retrieving Server Language - chrissstrahl
//--------------------------------------------------------------
str gameFixAPI_getServerLanguage()
{
	if (local_language->string) {
		return local_language->string;
	}
	return "Eng";
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function retrieving Player Language - chrissstrahl
//--------------------------------------------------------------
str gameFixAPI_getLanguage(Player* player)
{
	if (player) {
		if (gameFixAPI_inSingleplayer()) {
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
Entity* gameFixAPI_getActorFollowTargetEntity(Actor *actor) {
	if (!actor) { return nullptr; }
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
	return (Entity*)actor->followTarget.specifiedFollowTarget;
#else //FAKK2 MOHAA
	gi.Error(ERR_DROP, "gameFixAPI_getActorFollowTargetEntity - followTarget.specifiedFollowTarget NOT IN GAME\n");
#endif //GAME_STAR_TREK_ELITE_FORCE_2
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function checking if actor can actually see the given entity - chrissstrahl
//--------------------------------------------------------------
bool gameFixAPI_actorCanSee(Actor* actor, Entity* entity,bool useFOV,bool useVisionDistance)
{
	if (actor && entity){
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
		actor->sensoryPerception->CanSeeEntity((Entity*)actor, entity, useFOV, useVisionDistance);
#else //FAKK2 MOHAA
		if (useFOV)
		{
			actor->CanSeeFOV(entity);
		}
		else {
			actor->CanSee(entity);
		}
#endif //GAME_STAR_TREK_ELITE_FORCE_2
	}
	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function returning current enemy of given actor - chrissstrahl
//--------------------------------------------------------------
Entity* gameFixAPI_actorGetCurrentEnemy(Actor* actor)
{
	if (actor){
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
		return actor->enemyManager->GetCurrentEnemy();
#else //FAKK2 MOHAA
		return actor->currentEnemy;
#endif //GAME_STAR_TREK_ELITE_FORCE_2
	}
	return nullptr;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function cecking if actor hates given sentient - chrissstrahl
//--------------------------------------------------------------
bool gameFixAPI_actorHates(Actor* actor, Sentient *sentient)
{
	if (actor && sentient && gamefix_EntityValid((Entity*)sentient)) {
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
		return (bool)actor->enemyManager->Hates((Entity*)sentient);
#else //FAKK2 MOHAA
		return (bool)actor->Hates((Entity*)sentient);
#endif //GAME_STAR_TREK_ELITE_FORCE_2
	}
	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function checking if given player uses given weapon by name - chrissstrahl
//--------------------------------------------------------------
bool gameFixAPI_checkPlayerUsingWeaponNamed(Player* player, const str& weaponNameOfPlayer)
{
	bool checkForNone = false;
	if (!strlen(weaponNameOfPlayer.c_str()) || !Q_stricmp(weaponNameOfPlayer.c_str(), "none")) {
		checkForNone = true;
	}

	//nullptr,dead/spec/notarget - No active weapon check on - invalid player
	if (!gamefix_EntityValid(player) && checkForNone) {
		return true;
	}

	if (gamefix_EntityValid(player)) {
		//check for actual weapon match
		Weapon* pWeapon;
		pWeapon = player->GetActiveWeapon(WEAPON_DUAL);
		if (pWeapon && pWeapon->getName() == weaponNameOfPlayer)
			return true;

		pWeapon = player->GetActiveWeapon(WEAPON_LEFT);
		if (pWeapon && pWeapon->getName() == weaponNameOfPlayer)
			return true;

		pWeapon = player->GetActiveWeapon(WEAPON_RIGHT);
		if (pWeapon && pWeapon->getName() == weaponNameOfPlayer)
			return true;
		
		//No active weapon check on - valid player
		if (checkForNone && !pWeapon) {
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function returning number of player on the server - chrissstrahl
//--------------------------------------------------------------
int gameFixAPI_getPlayers(bool state)
{
	if (gameFixAPI_inMultiplayer()) {
		Player* player;
		int numPlayers = 0;
		for (int i = 0; i < gameFixAPI_maxClients(); i++){
			player = gamefix_getPlayer(i);
			if (player){
				if (!gameFixAPI_isSpectator_stef2(player) || !state){
					numPlayers++;
				}
			}
		}
		return numPlayers;
	}

	Player* player = gamefix_getPlayer(0);
	if (player) { return 1; }
	return 0;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to print text to all players - chrissstrahl
//--------------------------------------------------------------
void gameFixAPI_hudPrintAllClients(const str text)
{
	if (gameFixAPI_inMultiplayer()) {
		Player* player;
		for (int i = 0; i < gameFixAPI_maxClients(); i++){
			player = gamefix_getPlayer(i);
			if (player){
				gameFixAPI_hudPrint(player,text);
			}
		}
	}
	else {
		gentity_t* edict = nullptr;
		if (!edict) { return; }
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
		gi.centerprintf(edict,CENTERPRINT_IMPORTANCE_CRITICAL,text.c_str());
#else //FAKK2 MOHAA
		gi.centerprintf(edict,text.c_str());
#endif //GAME_STAR_TREK_ELITE_FORCE_2
	}
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function grabbing given entity, like in scripts - chrissstrahl
//--------------------------------------------------------------
Entity* gameFixAPI_getEntity(str &name)
{
	TargetList* tlist;
	if ((name.length() > 0) && name[0] == '*')
	{
		return G_GetEntity(atoi(&name[1]));
	}
	tlist = world->GetTargetList(name, false);

#ifndef GAME_STAR_TREK_ELITE_FORCE_2
	gi.Error(ERR_DROP,"gameFixAPI_getEntity - GetNextEntity differs from ELITE FORCE 2, please fix");
#endif //GAME_STAR_TREK_ELITE_FORCE_2

	if (tlist){
		return tlist->GetNextEntity(NULL);
	}

	return nullptr;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function manage level related fixes - chrissstrahl
//--------------------------------------------------------------
void gameFixAPI_levelFixes()
{
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
	if (Q_stricmpn(level.mapname, "m11l3a-drull_ruins3_boss", 24) == 0) {
		gameFixAPI_levelfix_m11l3a_drull_ruins3_boss();
	}
#endif //GAME_STAR_TREK_ELITE_FORCE_2

	if (!gameFixAPI_inMultiplayer()){
		return;
	}

#ifdef GAME_STAR_TREK_ELITE_FORCE_2
	if (Q_stricmpn(level.mapname, "dm_ctf_voy1", 11) == 0) {
		gameFixAPI_spawnlocations_dm_ctf_voy1();
	}
	else if(Q_stricmpn(level.mapname, "ctf_grey", 8) == 0) {
		gameFixAPI_maxLevelitems_ctf_grey();
	}
	else if(Q_stricmpn(level.mapname, "swsglobe", 8) == 0) {
		gameFixAPI_levelfix_swsglobe();
	}
#endif //GAME_STAR_TREK_ELITE_FORCE_2
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to fix warning message spam on ctf_grey - chrissstrahl
// Level: ctf_grey
//--------------------------------------------------------------
void gameFixAPI_maxLevelitems_ctf_grey()
{
	cvar_t* cvar = gi.cvar_get("max_levelitems");
	int maxLevelitems = (cvar ? cvar->integer : 0);
	if (maxLevelitems < 384) {
		gi.cvar_set("max_levelitems", "384");
		gi.SendServerCommand((int)NULL, "map ctf_grey");
		gi.Printf(_GFix_INFO_MapError,_GFixEF2_INFO_GAMEFIX_maxLevelitems_ctf_grey);
	}
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to fix spawnlocations on dm_ctf_voy1 - chrissstrahl
// Level: dm_ctf_voy1
//--------------------------------------------------------------
void gameFixAPI_spawnlocations_dm_ctf_voy1()
{
	//check if server has the script based fix installed - comes with the coop mod 6.xxxx and can be downloaded seperately
	//if (gamefix_functionExists("isPowerOfGiven")) {return;}

	//The Spawnlocations in the Level are broken, they are placed and targetnamed, but they have no origin
	//This makes players spawn outside at 0 0 0 during HM/TDM (mp_gametype 0/1)
	//This fixes this issue by moving the spawnloacations
	Entity* ent;
	Vector spawnlocations[64];
	spawnlocations[0] = Vector(4672.0f, -4352.0f, -384.0f);
	spawnlocations[1] = Vector(4224.0f, -3584.0f, 64.0f);
	spawnlocations[2] = Vector(4416.0f, -4352.0f, -384.0f);
	spawnlocations[3] = Vector(4288.0f, -4352.0f, -384.0f);
	spawnlocations[4] = Vector(4800.0f, -4352.0f, -384.0f);
	spawnlocations[5] = Vector(4224.0f, -3584.0f, -128.0f);
	spawnlocations[6] = Vector(4544.0f, -4352.0f, -384.0f);
	spawnlocations[7] = Vector(5184.0f, -2752.0f, -384.0f);
	spawnlocations[8] = Vector(5056.0f, -2752.0f, -384.0f);
	spawnlocations[9] = Vector(2912.0f, -3840.0f, -144.0f);
	spawnlocations[10] = Vector(2976.0f, -3968.0f, -144.0f);
	spawnlocations[11] = Vector(5280.0f, -3648.0f, -144.0f);
	spawnlocations[12] = Vector(5216.0f, -3776.0f, -144.0f);
	spawnlocations[13] = Vector(4672.0f, -4096.0f, -384.0f);
	spawnlocations[14] = Vector(4800.0f, -4096.0f, -384.0f);
	spawnlocations[15] = Vector(4800.0f, -4096.0f, -384.0f);
	spawnlocations[16] = Vector(3520.0f, -4352.0f, -384.0f);
	spawnlocations[17] = Vector(3776.0f, -4352.0f, -384.0f);
	spawnlocations[18] = Vector(3904.0f, -4352.0f, -384.0f);
	spawnlocations[19] = Vector(3392.0f, -4352.0f, -384.0f);
	spawnlocations[20] = Vector(3648.0f, -4352.0f, -384.0f);
	spawnlocations[21] = Vector(3520.0f, -4096.0f, -384.0f);
	spawnlocations[22] = Vector(3392.0f, -4096.0f, -384.0f);
	spawnlocations[23] = Vector(3968.0f, -3584.0f, 64.0f);
	spawnlocations[24] = Vector(3968.0f, -3584.0f, -128.0f);
	spawnlocations[25] = Vector(5056.0f, -2496.0f, -384.0f);
	spawnlocations[26] = Vector(4928.0f, -2496.0f, -384.0f);
	spawnlocations[27] = Vector(4928.0f, -2752.0f, -384.0f);
	spawnlocations[28] = Vector(3008.0f, -2752.0f, -384.0f);
	spawnlocations[29] = Vector(3136.0f, -2752.0f, -384.0f);
	spawnlocations[30] = Vector(3136.0f, -2496.0f, -384.0f);
	spawnlocations[31] = Vector(3264.0f, -2496.0f, -384.0f);
	spawnlocations[32] = Vector(3264.0f, -2752.0f, -384.0f);
	spawnlocations[33] = Vector(3520.0f, 3904.0f, -384.0f);
	spawnlocations[34] = Vector(3968.0f, 3136.0f, 64.0f);
	spawnlocations[35] = Vector(3776.0f, 3904.0f, -384.0f);
	spawnlocations[36] = Vector(3904.0f, 3904.0f, -384.0f);
	spawnlocations[37] = Vector(3392.0f, 3904.0f, -384.0f);
	spawnlocations[38] = Vector(3968.0f, 3136.0f, -128.0f);
	spawnlocations[39] = Vector(3648.0f, 3904.0f, -384.0f);
	spawnlocations[40] = Vector(3008.0f, 2304.0f, -384.0f);
	spawnlocations[41] = Vector(3136.0f, 2304.0f, -384.0f);
	spawnlocations[42] = Vector(5280.0f, 3392.0f, -144.0f);
	spawnlocations[43] = Vector(5216.0f, 3520.0f, -144.0f);
	spawnlocations[44] = Vector(2912.0f, 3200.0f, -144.0f);
	spawnlocations[45] = Vector(2976.0f, 3328.0f, -144.0f);
	spawnlocations[46] = Vector(3520.0f, 3648.0f, -384.0f);
	spawnlocations[47] = Vector(3392.0f, 3648.0f, -384.0f);
	spawnlocations[48] = Vector(4672.0f, 3904.0f, -384.0f);
	spawnlocations[49] = Vector(4416.0f, 3904.0f, -384.0f);
	spawnlocations[50] = Vector(4288.0f, 3904.0f, -384.0f);
	spawnlocations[51] = Vector(4800.0f, 3904.0f, -384.0f);
	spawnlocations[52] = Vector(4544.0f, 3904.0f, -384.0f);
	spawnlocations[53] = Vector(4672.0f, 3648.0f, -384.0f);
	spawnlocations[54] = Vector(4800.0f, 3648.0f, -384.0f);
	spawnlocations[55] = Vector(4224.0f, 3136.0f, 64.0f);
	spawnlocations[56] = Vector(4224.0f, 3136.0f, -128.0f);
	spawnlocations[57] = Vector(3136.0f, 2048.0f, -384.0f);
	spawnlocations[58] = Vector(3264.0f, 2048.0f, -384.0f);
	spawnlocations[59] = Vector(3264.0f, 2304.0f, -384.0f);
	spawnlocations[60] = Vector(5184.0f, 2304.0f, -384.0f);
	spawnlocations[61] = Vector(5056.0f, 2304.0f, -384.0f);
	spawnlocations[62] = Vector(5056.0f, 2048.0f, -384.0f);
	spawnlocations[63] = Vector(4928.0f, 2048.0f, -384.0f);
	
	Vector vAngle = Vector(0.0f,0.0f,0.0f);
	for (int i = 0; i < 64;i++) {
		str targetname = va("sp%d", i);
		ent = gameFixAPI_getEntity(targetname);
		if (ent) {
			if (i < 32) {
				vAngle[1] = 90.f;
			}
			else {
				vAngle[1] = 270.f;
			}
			ent->setAngles(vAngle);
			ent->setOrigin( spawnlocations[i]);
		}
	}

	gi.Printf(_GFix_INFO_APPLIED, _GFixEF2_INFO_GAMEFIX_spawnlocations_dm_ctf_voy1);
}

//--------------------------------------------------------------
// GAMEFIX - Added: script_model to fix a hole in floor making player fall out of level - chrissstrahl
// Level: m11l3a-drull_ruins3_boss
// Adds a clip to a specific location at the level start at the pillar where we fight the first stalker in the level
// Player can fall out of the level if circeling the pillar to closely, this clip fixes this - SP/MP
//--------------------------------------------------------------
void gameFixAPI_levelfix_m11l3a_drull_ruins3_boss()
{
	Entity *ent = gamefix_spawn("script_model", "fx/fx-dummy.tik", "-1277 17591 -152","",0);
	if (ent) {
		ent->setSize(Vector(-200.0f, -200.0f, -32.0f), Vector(200.0f, 200.0f, 1.0f));
		ent->setSolidType(SOLID_BBOX);
	}
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to fix swsglobe - chrissstrahl
// Level: swsglobe
// Disabled actor ai - actors twitched and tried to execute ai behaviours
// Removed snow effect it was causing performance issues - Out of Tempmodels
// Added push triggers
//--------------------------------------------------------------
extern Event EV_TriggerPushAny_SetSpeed;
void gameFixAPI_levelfix_swsglobe()
{
	//check if server has a script with a fix - exit if it does
	CThread* pThread;
	pThread = Director.CreateThread("removeSnow");
	if (pThread){
		return;
	}

	//server does not have a fix script
	for (int i = maxclients->integer; i < maxentities->integer;i++) {
		Entity* ent = G_GetEntity(i);
		if (ent) {
			//disable all actor AI
			if (ent->isSubclassOf(Actor)) {
				Actor *actor = (Actor*)ent;
				actor->TurnAIOff();
			}
			//remove snow - it tanks the performance real bad
			if (ent->model == "models/fx/fx-fallingsnow-movieset.tik") {
				ent->PostEvent(Event(EV_Remove), 0.0f);
			}
		}
	}

	//spawn triggers to simulate effect of the slopes with Patch 1.1 gamex86.dll and prevent falldamage
	Entity* ent = gamefix_spawn("trigger_pushany", "", "7528 -6140 -150", "", 0);
	if (ent) {
		ent->setSize(Vector(-68, -16, 0), Vector(68, 16, 24));
		ent->setAngles(Vector(350, 270, 0));
		TriggerPushAny *trigger = (TriggerPushAny*)ent;
		Event *ev = new Event(EV_TriggerPushAny_SetSpeed);
		ev->AddInteger(950);
		trigger->PostEvent(ev, 0.0f);	
	}
	ent = gamefix_spawn("trigger_pushany", "", "7528 -5284 -150", "", 0);
	if (ent) {
		ent->setSize(Vector(-68, -16, 0), Vector(68, 16, 24));
		ent->setAngles(Vector(350, 90, 0));
		TriggerPushAny* trigger = (TriggerPushAny*)ent;
		Event* ev = new Event(EV_TriggerPushAny_SetSpeed);
		ev->AddInteger(950);
		trigger->PostEvent(ev, 0.0f);
	}
	ent = gamefix_spawn("trigger_pushany", "", "7924 -5712 -150", "", 0);
	if (ent) {
		ent->setSize(Vector(-16, -68, 0), Vector(16, 68, 24));
		ent->setAngles(Vector(350, 360, 0));
		TriggerPushAny* trigger = (TriggerPushAny*)ent;
		Event* ev = new Event(EV_TriggerPushAny_SetSpeed);
		ev->AddInteger(950);
		trigger->PostEvent(ev, 0.0f);
	}
	ent = gamefix_spawn("trigger_pushany", "", "7124 -5712 -150", "", 0);
	if (ent) {
		ent->setSize(Vector(-16, -68, 0), Vector(16, 68, 24));
		ent->setAngles(Vector(350, 180, 0));
		TriggerPushAny* trigger = (TriggerPushAny*)ent;
		Event* ev = new Event(EV_TriggerPushAny_SetSpeed);
		ev->AddInteger(950);
		trigger->PostEvent(ev, 0.0f);
	}
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to get Owner of given entity - chrissstrahl
//--------------------------------------------------------------
Sentient* gamefixAPI_getOwner(Entity* entity)
{
	if (entity) {
		Sentient* sentient = nullptr;
		if (entity->isSubclassOf(Item)) {
			Item* equipment = (Equipment*)entity;
			sentient = equipment->GetOwner();
			if (sentient->isSubclassOf(Player)) {
				return sentient;
			}
		}
		else if (entity->isSubclassOf(PushObject)) {
			PushObject* pushObject = (PushObject*)entity;
			sentient = (Sentient*)pushObject->getOwner();
			if (sentient->isSubclassOf(Player)) {
				return sentient;
			}
		}
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
		if (entity->isSubclassOf(Equipment)) {
			Equipment* equipment = (Equipment*)entity;
			sentient = equipment->GetOwner();
			if (sentient->isSubclassOf(Player)) {
				return sentient;
			}
		}
#endif // GAME_STAR_TREK_ELITE_FORCE_2
	}
	return nullptr;
}



//--------------------------------------------------------------
// GAMEFIX - Added: Function to handle activation time in Multiplayer - chrissstrahl
//--------------------------------------------------------------
float gameFixAPI_getActivationTime(Entity* entity)
{
	if (gameFixAPI_inSingleplayer()) {
		gi.Error(ERR_DROP, "gameFixAPI_getActivationTime - Used in Singleplayer\n");
		return -1.0f;
	}
	if (!entity) {
		return 99999.0f;
	}
	return gamefix_entity_extraData_t[entity->entnum].lastActivated;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to handle activator in Multiplayer - chrissstrahl
//--------------------------------------------------------------
Player* gameFixAPI_getActivator(Entity* entity)
{
	if (entity) {
		//if (entity->isSubclassOf(Trigger)) {
			//Trigger* trigger = (Trigger*)entity;
			//Entity* activator = (Entity*)trigger->getActivator();
			//Entity* activator = (Entity*)trigger->activator;
		//}

		if (gameFixAPI_inSingleplayer()) {
			return (Player*)g_entities[0].entity;
		}

		Player *player = (Player*)(Entity*)gamefix_entity_extraData_t[entity->entnum].activator;

		if (!gameFixAPI_isDead((Entity*)player) && !gameFixAPI_isSpectator_stef2((Entity*)player)) {
			return player;
		}		
	}
	return nullptr;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to handle activator in Multiplayer - chrissstrahl
//--------------------------------------------------------------
void gameFixAPI_setActivator(Entity* entity, Entity* activator)
{
	if (entity && gameFixAPI_inMultiplayer()) {
		if (activator) {
#ifdef GAME_STAR_TREK_ELITE_FORCE_2
			if (activator->isSubclassOf(Equipment)) {
				Equipment* equipment = (Equipment*)activator;
				activator = equipment->GetOwner();
				if (!activator || !activator->isSubclassOf(Sentient)) {
					activator = nullptr;
				}
			}
#endif // GAME_STAR_TREK_ELITE_FORCE_2

			if (activator && activator->isSubclassOf(Player)) {
				if (gameFixAPI_isDead(activator) || gameFixAPI_isSpectator_stef2(activator)) {
					activator = nullptr;
				}
			}

			if (activator) {
				gamefix_entity_extraData_t[entity->entnum].lastActivated = level.time;
			}
		}
		
		gamefix_entity_extraData_t[entity->entnum].activator = (EntityPtr)activator;
	}
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to manage maplist in Multiplayer, because the actual functions are not as flexible as we need them to be - chrissstrahl
//--------------------------------------------------------------
int gameFixAPI_mapListGetCurrentPos()
{
	int currentPos = gamefix_getCvarInt("mp_currentPosInMapList");
	int iCount = gameFixAPI_mapListCount();
	if (iCount > 0) {
		currentPos = (currentPos % iCount);
	}
	return currentPos;
}

str gameFixAPI_mapListUp()
{
	int currentPos = gameFixAPI_mapListGetCurrentPos();
	currentPos++;
	gi.cvar_set("mp_currentPosInMapList", va("%d", currentPos));
	return gameFixAPI_mapListGetAtPos(currentPos);
}

str gameFixAPI_mapListDown()
{
	int currentPos = gameFixAPI_mapListGetCurrentPos();
	currentPos--;
	if (currentPos < 0) {
		currentPos = (gameFixAPI_mapListCount() - 1);
	}
	gi.cvar_set("mp_currentPosInMapList", va("%d", currentPos));
	return gameFixAPI_mapListGetAtPos(currentPos);
}

str gameFixAPI_mapListGetAtPos(int currentPos)
{
	int iPos = currentPos;
	iPos++; //mp_currentPosInMapList -> starts at 0, container list at 1, compensate offset
	int count = gameFixAPI_mapListCount();

	//out of bounds
	if (count > 0) {
		if (iPos > count) { iPos = 1; }
		else if (iPos < 1) { iPos = count; }

		return gameFixAPI_maplistContainer.ObjectAt(iPos).c_str();
	}
	return level.mapname;
}

int gameFixAPI_mapListCount()
{
	return gameFixAPI_maplistContainer.NumObjects();
}

void gameFixAPI_mapList()
{
	gameFixAPI_maplistContainer.FreeObjectList();

	str mapList = mp_mapList->string;
	int maplistStrLength = strlen(mapList.c_str());
	if (maplistStrLength) {
		int currentCharPos = 0;
		str mapname = "";
		bool addMap = false;

		while (currentCharPos < maplistStrLength || addMap == true) {
			if (addMap) {
				if (strlen(mapname.c_str())) {
					if (gi.FS_Exists(va("maps/%s.bsp", mapname.c_str()))) {
						gameFixAPI_maplistContainer.AddObject(mapname);
					}
					else {
						gi.Printf("mp_mapList: maps/%s.bsp does not exist on server\n", mapname.c_str());
					}
					mapname = "";
				}
				addMap = false;
				if (currentCharPos == maplistStrLength) {
					break;
				}
			}

			if (mapList[currentCharPos] == ';') {
				addMap = true;
			}
			else if (mapList[currentCharPos] != ' ') {
				mapname += mapList[currentCharPos];
			}

			currentCharPos++;

			if (currentCharPos == maplistStrLength) {
				addMap = true;
			}
		}

		int numObjects = gameFixAPI_maplistContainer.NumObjects();
		int curObj = 1;
		while (curObj <= numObjects) {
			gi.Printf("map in list: %s at %d\n", gameFixAPI_maplistContainer.ObjectAt(curObj).c_str(), curObj);
			curObj++;
		}
	}
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to manage Actor-Dialog in Multiplayer - chrissstrahl
//--------------------------------------------------------------
void gameFixAPI_dialogSetupPlayers(Actor* speaker, char localizedDialogName[MAX_QPATH], bool headDisplay)
{
	Player* player = nullptr;
	Actor* eActor = NULL;

#ifdef GAME_STAR_TREK_ELITE_FORCE_2
	if (headDisplay && speaker) {
		eActor = speaker;
		eActor->SetActorFlag(ACTOR_FLAG_USING_HUD, true);
	}
	for (int i = 0; i < gameFixAPI_maxClients(); i++) {
		player = gamefix_getPlayer(i);
		if (player && player->client && player->isSubclassOf(Player)) {
			player->SetupDialog(eActor, localizedDialogName);
		}
	}
#endif //GAME_STAR_TREK_ELITE_FORCE_2
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to create default maps list - chrissstrahl
//--------------------------------------------------------------
static void gameFixAPI_addDefaultMaps()
{
	gameFixAPI_addMap("ent-training1", "sp", "training");
	gameFixAPI_addMap("ent-training2", "sp", "training");
	gameFixAPI_addMap("ent-training3", "sp", "training");
	gameFixAPI_addMap("ent-training4", "sp", "training");

	gameFixAPI_addMap("m0-intro", "sp", "cinematic");
	gameFixAPI_addMap("m2l0-sfa", "sp", "cinematic");
	gameFixAPI_addMap("m6l0-attack", "sp", "cinematic");
	gameFixAPI_addMap("m12-end", "sp", "cinematic");
	gameFixAPI_addMap("credits", "sp", "cinematic");

	gameFixAPI_addMap("blackbox", "sp", "test");
	gameFixAPI_addMap("ent-deck", "sp", "test");

	gameFixAPI_addMap("m1l1a-borg_sphere", "sp", "mission");
	gameFixAPI_addMap("m1l1b-borg_sphere", "sp", "mission");
	gameFixAPI_addMap("m1l1c-borg_sphere", "sp", "mission");
	gameFixAPI_addMap("m1l2a-borg_sphere", "sp", "mission");
	gameFixAPI_addMap("m1l2b-borg_sphere", "sp", "mission");
	gameFixAPI_addMap("m1l2c-borg_sphere", "sp", "mission");
	gameFixAPI_addMap("m1l3-borg_boss", "sp", "mission");
	gameFixAPI_addMap("m2l1-sfa", "sp", "mission");
	gameFixAPI_addMap("m2l2-sfa", "sp", "mission");
	gameFixAPI_addMap("m3l1a-forever", "sp", "mission");
	gameFixAPI_addMap("m3l1b-forever", "sp", "mission");
	gameFixAPI_addMap("m3l2-forever", "sp", "mission");
	gameFixAPI_addMap("m4l1a-attrexian_station", "sp", "mission");
	gameFixAPI_addMap("m4l1b-attrexian_station", "sp", "mission");
	gameFixAPI_addMap("m4l2b-attrexian_station", "sp", "mission");
	gameFixAPI_addMap("m5l1a-drull_ruins1", "sp", "mission");
	gameFixAPI_addMap("m5l1b-drull_ruins1", "sp", "mission");
	gameFixAPI_addMap("m5l2a-drull_ruins1", "sp", "mission");
	gameFixAPI_addMap("m5l2b-drull_ruins1", "sp", "mission");
	gameFixAPI_addMap("m5l2c-drull_ruins1", "sp", "mission");
	gameFixAPI_addMap("m6-deck8_redalert", "sp", "mission");
	gameFixAPI_addMap("m6-deck16_engineering", "sp", "mission");
	gameFixAPI_addMap("m6-deck1_bridge_redalert", "sp", "mission");
	gameFixAPI_addMap("m6-exterior", "sp", "mission");
	gameFixAPI_addMap("m7l1a-attrexian_colony", "sp", "mission");
	gameFixAPI_addMap("m7l1b-attrexian_colony", "sp", "mission");
	gameFixAPI_addMap("m7l2a-attrexian_colony", "sp", "mission");
	gameFixAPI_addMap("m7l2b-attrexian_colony", "sp", "mission");
	gameFixAPI_addMap("m7l2c-attrexian_colony", "sp", "mission");
	gameFixAPI_addMap("m8l1a-drull_ruins2", "sp", "mission");
	gameFixAPI_addMap("m8l1b-drull_ruins2", "sp", "mission");
	gameFixAPI_addMap("m8l2a-drull_ruins2", "sp", "mission");
	gameFixAPI_addMap("m8l2b-drull_ruins2", "sp", "mission");
	gameFixAPI_addMap("m9l1a-klingon_base", "sp", "mission");
	gameFixAPI_addMap("m9l1b-klingon_base", "sp", "mission");
	gameFixAPI_addMap("m9l2-klingon_base", "sp", "mission");
	gameFixAPI_addMap("m10l1-romulan_installation", "sp", "mission");
	gameFixAPI_addMap("m10l2a-romulan_installation", "sp", "mission");
	gameFixAPI_addMap("m10l2b-romulan_installation", "sp", "mission");
	gameFixAPI_addMap("m11l1a-drull_ruins3", "sp", "mission");
	gameFixAPI_addMap("m11l2a-drull_ruins3", "sp", "mission");
	gameFixAPI_addMap("m11l3a-drull_ruins3_boss", "sp", "mission");
	gameFixAPI_addMap("m11l3b-drull_ruins3_boss", "sp", "mission");

	gameFixAPI_addMap("ent-training_weap1", "sp", "mission");
	gameFixAPI_addMap("ent-training_weap2", "sp", "mission");
	gameFixAPI_addMap("ent-training_weap3", "sp", "mission");
	gameFixAPI_addMap("ent-training_weap4", "sp", "mission");

	gameFixAPI_addMap("ent-deck11", "sp", "igm");
	gameFixAPI_addMap("ent-deck16_engineering", "sp", "igm");
	gameFixAPI_addMap("ent-deck7c", "sp", "igm");
	gameFixAPI_addMap("ent-deck7b", "sp", "igm");
	gameFixAPI_addMap("ent-deck8", "sp", "igm");
	gameFixAPI_addMap("ent-deck7a", "sp", "igm");
	gameFixAPI_addMap("ent-deck1_bridge", "sp", "igm");

	gameFixAPI_addMap("secret1", "sp", "secret");
	gameFixAPI_addMap("secret2", "sp", "secret");
	gameFixAPI_addMap("ef2_minigame2", "sp", "secret");
	gameFixAPI_addMap("secret3", "sp", "secret");
	gameFixAPI_addMap("zoo1", "sp", "secret");
	gameFixAPI_addMap("testmorph", "sp", "secret");

	gameFixAPI_addMap("dm_attrexian1", "mp", "Holomatch TeamHolomatch controlpoints");
	gameFixAPI_addMap("dm_borgurvish", "mp", "Holomatch TeamHolomatch controlpoints");
	gameFixAPI_addMap("dm_bridwag", "mp", "Holomatch TeamHolomatch controlpoints");
	gameFixAPI_addMap("dm_idryll2", "mp", "Holomatch TeamHolomatch controlpoints");
	gameFixAPI_addMap("dm_kirk", "mp", "Holomatch TeamHolomatch controlpoints");
	gameFixAPI_addMap("dm_kw", "mp", "Holomatch TeamHolomatch controlpoints");
	gameFixAPI_addMap("dm_quarterdeck", "mp", "Holomatch TeamHolomatch");
	gameFixAPI_addMap("dm_sewer", "mp", "Holomatch TeamHolomatch controlpoints powerstruggle oneflag");
	gameFixAPI_addMap("dm_gullie", "mp", "Holomatch TeamHolomatch");
	gameFixAPI_addMap("dm_trophonius", "mp", "Holomatch TeamHolomatch controlpoints");

	gameFixAPI_addMap("dm_ctf_station", "mp", "Holomatch TeamHolomatch CaptureTheFlag BombDiffusion controlpoints powerstruggle oneflag specialties");
	gameFixAPI_addMap("dm_t2_new", "mp", "Holomatch TeamHolomatch CaptureTheFlag BombDiffusion controlpoints powerstruggle oneflag specialties");
	gameFixAPI_addMap("aah_ctf_tartirus", "mp", "Holomatch TeamHolomatch CaptureTheFlag BombDiffusion powerstruggle oneflag specialties");
	gameFixAPI_addMap("ctf_vor2", "mp", "Holomatch TeamHolomatch CaptureTheFlag controlpoints powerstruggle");
	gameFixAPI_addMap("dm_ctf_voy1", "mp", "Holomatch TeamHolomatch CaptureTheFlag BombDiffusion controlpoints");
	gameFixAPI_addMap("hm_ctf_exigent", "mp", "Holomatch TeamHolomatch CaptureTheFlag BombDiffusion controlpoints powerstruggle");
	gameFixAPI_addMap("dm_t4mobius", "mp", "Holomatch TeamHolomatch CaptureTheFlag BombDiffusion controlpoints powerstruggle oneflag specialties");
	
}
static bool gameFixAPI_mapIsStock(str name)
{
	int i = 0;
	while (i <= gamefix_defaultMapsSize) {
		//find map
		if (Q_stricmp(gamefix_defaultMaps_t[i].mapname.c_str(), name.c_str()) == 0) {
			return true;
		}
		i++;
	}
	return false;
}
static bool gameFixAPI_mapForSingleplayer(str name)
{
	int i = 0;
	while (i <= gamefix_defaultMapsSize) {
		//find map
		if (Q_stricmp(gamefix_defaultMaps_t[i].mapname.c_str(), name.c_str()) == 0) {
			//check for gametype
			if (Q_stricmp(gamefix_defaultMaps_t[i].gametypes.c_str(), "sp") == 0) {
				return true;
			}
		}
		i++;
	}
	return false;
}
static bool gameFixAPI_mapForMultiplayer(str name)
{
	int i = 0;
	while (i <= gamefix_defaultMapsSize) {
		//find map
		if (Q_stricmp(gamefix_defaultMaps_t[i].mapname.c_str(), name.c_str()) == 0) {
			//check for gametype
			if (Q_stricmp(gamefix_defaultMaps_t[i].gametypes.c_str(), "mp") == 0) {
				return true;
			}
		}
		i++;
	}
	return false;
}
static bool gameFixAPI_mapHasGameMode(str name,str gamemode)
{
	int i = 0;
	while (i <= gamefix_defaultMapsSize) {
		//find map
		if (Q_stricmp(gamefix_defaultMaps_t[i].mapname.c_str(), name.c_str()) == 0) {
			//check for given gamemode
			if (gamefix_findString(gamefix_defaultMaps_t[i].gamemodes.c_str(), gamemode.c_str()) != -1) {
				return true;
			}
		}
		i++;
	}
	return false;
}
static void gameFixAPI_addMap(str name, str gametypes, str gamemodes)
{
	static int mapIndex = 0;
	gamefix_defaultMaps_t[mapIndex].mapname = name;
	gamefix_defaultMaps_t[mapIndex].gametypes = gametypes;
	gamefix_defaultMaps_t[mapIndex].gamemodes = gamemodes;
	mapIndex++;
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to manage game shutdown - chrissstrahl
//--------------------------------------------------------------
void gameFixAPI_shutdownGame()
{
	gameFixAPI_maplistContainer.FreeObjectList();

	/* int i = 0;
	while (i < 89) {
		gi.Printf("%d: %s\n",i,gamefix_defaultMaps_t[i].mapname.c_str());
		i++;
	}*/
}

//--------------------------------------------------------------
// GAMEFIX - Added: Function to manage game startup - chrissstrahl
//--------------------------------------------------------------
void gameFixAPI_initGame()
{
	gameFixAPI_addDefaultMaps();
}
