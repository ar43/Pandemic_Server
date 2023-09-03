#include "server.h"

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <array>
#include <algorithm>
#include <memory>

#include "client.h"
#include "game.h"

#include "spdlog/spdlog.h"
#include "nlohmann/json.hpp"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

Server::Server()
{
	std::fill(begin(taken_ids), end(taken_ids), false);
}

void Server::CreateGames()
{
	auto game0 = std::make_unique<Game>(1, GenerateGameId(), true);
	auto game1 = std::make_unique<Game>(2, GenerateGameId(), true);
	auto game2 = std::make_unique<Game>(3, GenerateGameId(), true);
	auto game3 = std::make_unique<Game>(4, GenerateGameId(), true);

	games.push_back(std::move(game0));
	games.push_back(std::move(game1));
	games.push_back(std::move(game2));
	games.push_back(std::move(game3));
}

int Server::GenerateGameId()
{
	for (int i = 0; i < taken_ids.size(); i++)
	{
		if (taken_ids[i] == false)
		{
			taken_ids[i] = true;
			return i;
		}
	}
	return -1;
}

bool Server::Init()
{
    WSADATA wsaData;
    int iResult;

    listen_socket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        spdlog::error("WSAStartup failed with error: {}", iResult);
        return false;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, std::to_string(DEFAULT_PORT).c_str(), &hints, &result);
    if ( iResult != 0 ) {
        spdlog::error("getaddrinfo failed with error: {}", iResult);
        WSACleanup();
        return true;
    }

    // Create a SOCKET for the server to listen for client connections.
    listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        spdlog::error("socket failed with error: {}", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return true;
    }

    u_long mode = 1;  // 1 to enable non-blocking socket
    ioctlsocket(listen_socket, FIONBIO, &mode);

    // Setup the TCP listening socket
    iResult = bind( listen_socket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        spdlog::error("bind failed with error: {}", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listen_socket);
        WSACleanup();
        return false;
    }

    freeaddrinfo(result);

    iResult = listen(listen_socket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        spdlog::error("listen failed with error: {}", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return false;
    }

	CreateGames();

    return true;
    //FD_ZERO(&master_set);
}

void Server::Run()
{
    running = true;

    while (running)
    {
        Tick();
        Sleep(100);
    }
}

void Server::Tick()
{
    AcceptNewConnections();
    ReadClientData();
    UpdateAwaitingClients();
    UpdatePlayers();
	UpdateGame();
    WriteClientData();
}

void Server::AcceptNewConnections()
{
    SOCKET new_socket = INVALID_SOCKET;
    new_socket = accept(listen_socket, NULL, NULL);
    if (new_socket != INVALID_SOCKET) 
    {
        auto new_client = std::make_unique<Client>(new_socket,&rng);
        awaiting_clients.push_back(std::move(new_client));
        spdlog::info("AcceptNewConnections: new client connected");
    }
}

void Server::UpdatePlayers()
{
	for (auto& game : games)
	{
		for (auto& it : game->players)
		{
			it->Update();
		}

		for (auto it = game->players.begin(); it != game->players.end();)
		{
			if ((*it)->dropped)
			{
				if((*it)->state == CSTATE_GAME)
					spdlog::warn("dropped client {} while ingame", (*it)->GetPid());
				closesocket((*it)->socket);
				it = game->players.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
}

void Server::UpdateGame()
{
	for (auto& game : games)
	{
		game->Update();
	}
}

void Server::ReadClientData()
{
    for (auto& it : awaiting_clients)
    {
        it->ReadInput();
    }
	for (auto& game : games)
	{
		for (auto& it : game->players)
		{
			it->ReadInput();
		}
	}
}

void Server::UpdateAwaitingClients()
{
    for (auto it = awaiting_clients.begin(); it != awaiting_clients.end();)
    {
        int requested_lobby = (*it)->UpdateAwaiting();
        if (requested_lobby >= 0)
        {
			bool flag1 = false;
			bool flag2 = false;
			for (auto& game : games)
			{
				if (game->GetId() == requested_lobby)
				{
					if (!game->IsInProgress())
					{
						(*it)->SetPid(game->GeneratePid());
						(*it)->AddToLobby(game->GetId());
						game->players.push_back(std::move(*it));
						it = awaiting_clients.erase(it);
						game->StartIfFull();
						flag1 = true;
					}
					else
					{
						(*it)->awaiting_substate = 2; //Game full
						flag2 = true;
					}
				}
			}
			if(!flag2 && !flag1)
				(*it)->awaiting_substate = 3; //Game does not exist
			if(!flag1)
				it++;
			
        }
        else if ((*it)->dropped)
        {
            closesocket((*it)->socket);
            it = awaiting_clients.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void Server::WriteClientData()
{
    for (auto& it : awaiting_clients)
    {
		it->SendOutput();
    }
	for (auto& game : games)
	{
		for (auto& it : game->players)
		{
			it->SendOutput();
		}
	}
}
