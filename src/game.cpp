#include "game.h"
#include "spdlog/spdlog.h"
#include "client.h"
#include "client_input.h"
#include "opcode_manager.h"
#include "out_update_players.h"
#include "out_begin_game.h"
#include "out_server_message.h"
#include "client_message_type.h"
#include "nlohmann/json.hpp"
#include "map.h"

#include <iostream>
#include <array>
#include <format>

Game::Game(uint8_t max_players, uint8_t id, bool auto_restart)
{
	this->max_players = max_players;
	std::fill(begin(positions), end(positions), 0);
	this->id = id;
	this->auto_restart = auto_restart;
	this->in_progress = false;
}

Game::~Game()
{
}

bool Game::IsInProgress()
{
	return in_progress;
}

void Game::BroadcastPositions()
{
	if (broadcast_positions)
	{
		OutUpdatePlayers update_players((uint8_t)players.size(), positions.data());
		Broadcast(update_players);
		broadcast_positions = false;
	}
}

void Game::Update()
{
	if (!IsInProgress())
		return;

	if (players.size() != max_players)
	{
		Kill("player disconnected");
		return;
	}

	ProcessInput();
	UpdateGameState();
	BroadcastPositions();

	time++;
}

void Game::StartIfFull()
{
	if (players.size() == max_players)
	{
		in_progress = true;
		LoadMap("map_default.json");
		spdlog::info("started game with id {}", GetId());
		for (auto& player : players)
		{
			player->state = ClientState::CSTATE_GAME;
			OutBeginGame out_begin_game((uint8_t)players.size(), player->GetPid());
			player->opcode_manager->Send(out_begin_game);
			broadcast_positions = true;
		}
	}
}

bool Game::RequestedKill()
{
	return request_kill;
}

void Game::Kill(std::string reason)
{
	request_kill = true;
	std::string str = std::string("Game killed - reason: ") + reason;
	OutServerMessage msg(ServerMessageType::SMESSAGE_INFO, str);
	Broadcast(msg);
}

bool Game::LoadMap(std::string map_name)
{
	current_map = std::make_unique<Map>(map_name);
	return true;
}

uint8_t Game::GetId()
{
	return id;
}

void Game::UpdateGameState()
{
	for (auto& player : players)
	{
		positions[player->GetPid()] = player->position;
	}
}

void Game::ProcessInput()
{
	for (auto& player : players)
	{
		auto &client_input = player->client_input;
		if (client_input->requested_move)
		{
			if (current_map->IsCityNeighbour(player->position, client_input->target_city))
			{
				spdlog::info("a player requested to move");
				broadcast_positions = true;
				player->position = client_input->target_city;
			}
			client_input->requested_move = false;
		}
		if (client_input->client_message != nullptr)
		{
			if (client_input->client_message->first == ClientMessageType::CMESSAGE_CHAT)
			{
				std::string msg = std::to_string(player->GetPid()) + std::string(": ") + client_input->client_message->second;
				//msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.cend());
				OutServerMessage chat_msg(ServerMessageType::SMESSAGE_CHAT, msg);
				Broadcast(chat_msg);
				client_input->client_message = nullptr;
			}
			
		}
	}
}

void Game::Broadcast(OpcodeOut& opcode)
{
	for (auto& player : players)
	{
		player->opcode_manager->Send(opcode);
	}
}

uint8_t Game::GeneratePid()
{
	if (players.size() >= max_players)
	{
		spdlog::error("server full");
		return 0;
	}

	return (uint8_t)players.size();
}

uint8_t Game::GetMaxPlayers()
{
	return max_players;
}
