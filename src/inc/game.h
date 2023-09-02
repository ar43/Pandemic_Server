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
	void Update();
	void Start();
	std::vector<std::shared_ptr<Client>> players;

private:
	
	void UpdateGameState();
	void ProcessInput();
	void Broadcast(OpcodeOut& opcode);
	bool in_progress = false;
	uint8_t positions[MAX_PLAYERS];
};