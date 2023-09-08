#pragma once
#include <vector>
#include <memory>
#include <array>
#include <string>

#include "randutils.hpp"

class Client;
class OpcodeOut;
class Map;
class Timer;

#define MAX_PLAYERS 4

class Game
{
public:
	Game(uint8_t max_players, uint8_t id, bool auto_restart);
	~Game();
	bool IsInProgress();
	void BroadcastPositions();
	void Update();
	void OnPlayerJoin(std::string player_name);
	void Kill(std::string reason);
	bool LoadMap(std::string map_name);
	bool RequestedKill();
	uint8_t GetId();
	uint8_t GeneratePid();
	uint8_t GetMaxPlayers();
	std::vector<std::unique_ptr<Client>> players;
	bool auto_restart;

private:
	void UpdateGameState();
	void ProcessInput();
	void Broadcast(OpcodeOut& opcode);
	void GenerateRoles();
	void ValidateNames();
	void Start();
	void ProcessClientMessages();
	
	bool in_progress = false;
	bool broadcast_positions = false;
	bool request_kill = false;
	
	uint64_t ticks = 0;
	uint8_t id;
	uint8_t max_players;
	std::array<uint8_t, MAX_PLAYERS> positions;
	std::unique_ptr<Map> current_map;
	std::unique_ptr<Timer> game_begin_timer;
	randutils::mt19937_rng rng;
};