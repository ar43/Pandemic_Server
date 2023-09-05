#pragma once
#include <vector>
#include <memory>
#include <array>
#include <string>

#include "randutils.hpp"

class Client;
class OpcodeOut;
class Map;

#define MAX_PLAYERS 4

class Game
{
public:
	Game(uint8_t max_players, uint8_t id, bool auto_restart);
	~Game();
	bool IsInProgress();
	void BroadcastPositions();
	void Update();
	void StartIfFull();
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
	
	bool in_progress = false;
	bool broadcast_positions = false;
	bool request_kill = false;
	
	uint64_t time = 0;
	std::array<uint8_t, MAX_PLAYERS> positions;
	uint8_t id;
	randutils::mt19937_rng rng;

	uint8_t max_players;

	std::unique_ptr<Map> current_map;
};