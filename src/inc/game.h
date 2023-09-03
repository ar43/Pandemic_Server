#pragma once
#include <vector>
#include <memory>
#include <array>

class Client;
class OpcodeOut;

#define MAX_PLAYERS 4

class Game
{
public:
	Game(uint8_t max_players, uint8_t id, bool auto_restart);
	bool IsInProgress();
	void BroadcastPositions();
	void Update();
	void StartIfFull();
	uint8_t GetId();
	uint8_t GeneratePid();
	std::vector<std::unique_ptr<Client>> players;

private:
	
	void UpdateGameState();
	void ProcessInput();
	void Broadcast(OpcodeOut& opcode);
	
	bool in_progress = false;
	bool broadcast_positions = false;
	bool auto_restart;
	uint64_t time = 0;
	std::array<uint8_t, MAX_PLAYERS> positions;
	uint8_t id;

	uint8_t max_players;
};