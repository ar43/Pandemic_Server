#pragma once
#include <vector>
#include <memory>

class Client;
class OpcodeOut;

#define MAX_PLAYERS 4

class Game
{
public:
	Game(std::vector<std::shared_ptr<Client>>* players);
	bool IsInProgress();
	void Update();
	void Start();

private:
	void UpdateGameState();
	void ProcessInput();
	void Broadcast(OpcodeOut& opcode);
	bool in_progress = false;
	std::vector<std::shared_ptr<Client>> *players;
	uint8_t positions[MAX_PLAYERS];
};