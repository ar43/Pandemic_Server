#pragma once
#include <vector>
#include <memory>

class Client;
class OpcodeOut;

#define MAX_PLAYERS 4

class Game
{
public:
	Game();
	bool IsInProgress();
	void BroadcastPositions();
	void Update();
	void Start();
	std::vector<std::shared_ptr<Client>> players;

private:
	
	void UpdateGameState();
	void ProcessInput();
	void Broadcast(OpcodeOut& opcode);
	bool in_progress = false;
	bool broadcast_positions = false;
	uint64_t time = 0;
	uint8_t positions[MAX_PLAYERS];
};