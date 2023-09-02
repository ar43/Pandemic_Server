#pragma once

#include <winsock2.h>
#include <vector>
#include <memory>

#include "randutils.hpp"

class Client;
class Game;

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
	uint8_t GeneratePid();

	bool running;
	SOCKET listen_socket;
	std::vector<std::shared_ptr<Client>> awaiting_clients;
	std::unique_ptr<Game> game;

	fd_set master_set, working_set;

	const int DEFAULT_PORT = 43594;
	size_t max_players = 2;
};