#pragma once
#include <vector>
#include <memory>
#include <array>
#include <string>
#include <cstdint>
#include <utility>

#include "randutils.hpp"
#include "infection_type.h"
#include "infection_card.h"

class Client;
class OpcodeOut;
class Map;
class Timer;
class CardStack;

constexpr auto MAX_PLAYERS = 4;

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
	void SendLobbyPlayerCount();

	void DrawInfectionCard(uint8_t infection_multiplier);
	void DebugInfect(InfectionCard target_card);
	void InfectCity(int city_id, uint8_t card_id, InfectionType type, uint8_t infection_multiplier, std::vector<std::pair<uint8_t, uint8_t>> &infection_data);
	
	bool in_progress = false;
	bool broadcast_positions = false;
	bool request_kill = false;
	
	uint64_t ticks = 0;
	uint8_t id;
	uint8_t max_players;
	std::array<uint8_t, MAX_PLAYERS> positions;
	std::unique_ptr<Map> current_map;
	std::unique_ptr<CardStack> player_card_deck;
	std::unique_ptr<CardStack> infection_card_deck;
	std::unique_ptr<CardStack> infection_card_discard_pile;
	std::unique_ptr<Timer> game_begin_timer;
	std::unique_ptr<Timer> lobby_player_count_timer;
	randutils::mt19937_rng rng;
};