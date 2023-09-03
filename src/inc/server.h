#pragma once

#include <winsock2.h>
#include <vector>
#include <memory>
#include <array>
#include <queue>

#include "randutils.hpp"

class Client;
class Game;

#define MAX_GAMES 256

class Server
{
public:
	Server();
	randutils::mt19937_rng rng;
	bool Init();
	void Run();
	
private:
	void Tick();
	void AcceptNewConnections();
	void ReadClientData();
	void UpdateAwaitingClients();
	void UpdatePlayers();
	void UpdateGame();
	void WriteClientData();
	void CreateQueuedGames();
	void QueueInitialGames();

	int GenerateGameId();

	bool running;
	SOCKET listen_socket;
	std::vector<std::unique_ptr<Client>> awaiting_clients;
	std::vector<std::unique_ptr<Game>> games;

	std::queue<std::unique_ptr<Game>> game_creation_queue;

	fd_set master_set, working_set;

	std::array<bool, MAX_GAMES> taken_ids;

	const int DEFAULT_PORT = 43594;
};