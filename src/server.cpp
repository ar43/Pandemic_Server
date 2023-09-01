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

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

Server::Server()
{
	game = std::make_unique<Game>(&players);
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
    WriteClientData();
}

void Server::AcceptNewConnections()
{
    SOCKET new_socket = INVALID_SOCKET;
    new_socket = accept(listen_socket, NULL, NULL);
    if (new_socket != INVALID_SOCKET) 
    {
		if (game->IsInProgress())
		{
			closesocket(new_socket);
			spdlog::info("AcceptNewConnections: rejected connection because the server is full");
			return;
		}
        auto new_client = std::make_shared<Client>(new_socket,&rng);
        awaiting_clients.push_back(new_client);
        spdlog::info("AcceptNewConnections: new client connected");
    }
}

void Server::ReadClientData()
{
    for (auto& it : awaiting_clients)
    {
        it->ReadInput();
    }
    for (auto& it : players)
    {
        it->ReadInput();
    }
}

void Server::UpdateAwaitingClients()
{
    
    for (auto it = awaiting_clients.begin(); it != awaiting_clients.end();)
    {
        (*it)->UpdateAwaiting((uint8_t)players.size(),(uint8_t)max_players);
        if ((*it)->state == CSTATE_LOBBY)
        {
			(*it)->SetPid(GeneratePid());
            players.push_back(*it);
            it = awaiting_clients.erase(it);
			if (players.size() == max_players)
			{
				game->Start();
			}
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

void Server::UpdatePlayers()
{
    for (auto& it : players)
    {
        it->Update();
    }

    for (auto it = players.begin(); it != players.end();)
    {
        if ((*it)->dropped)
        {
            closesocket((*it)->socket);
            it = players.erase(it);
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
    for (auto& it : players)
    {
        it->SendOutput();
    }
}

uint8_t Server::GeneratePid()
{
	if (players.size() >= max_players)
	{
		spdlog::error("server full");
		return 0;
	}
	
	return (uint8_t)players.size();
}
