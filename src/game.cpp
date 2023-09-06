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
#include "player_role.h"
#include "player_info.h"

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

void Game::GenerateRoles()
{
	std::vector<PlayerRole> role_list;
	for (int i = 0; i < (int)PlayerRole::NUM_ROLES; i++)
	{
		role_list.push_back((PlayerRole)i);
	}

	for (auto& player : players)
	{
		auto num = rng.uniform(0, (int)role_list.size()-1);
		if (num < 0 || num >(int)PlayerRole::NUM_ROLES)
		{
			spdlog::error("fix GenerateRoles");
			exit(1);
		}

		player->player_info->SetRole((PlayerRole)role_list.at(num));
		role_list.erase(role_list.begin()+num);
	}
}

void Game::ValidateNames()
{
	std::vector<std::string> names;
	for (auto& player : players)
	{
		auto player_name = player->player_info->GetName();
		for (auto name : names)
		{
			if (player_name == name)
			{
				player->player_info->SetName(std::format("{}({})", player_name, player->GetPid()));
			}
		}
		names.push_back(player_name);
	}
}

void Game::StartIfFull()
{
	if (players.size() == max_players)
	{
		in_progress = true;
		LoadMap("map_default.json");
		spdlog::info("started game with id {}", GetId());
		GenerateRoles();
		ValidateNames();
		std::vector<std::string> player_names;
		std::vector<PlayerRole> player_roles;
		for (auto& player : players)
		{
			player->player_info->SetActions(0);
			player->state = ClientState::CSTATE_GAME;
			player_names.push_back(player->player_info->GetName());
			player_roles.push_back(player->player_info->GetRole());
		}
		for (auto& player : players)
		{
			OutBeginGame out_begin_game((uint8_t)players.size(), player->GetPid(),&player_names,&player_roles);
			player->opcode_manager->Send(out_begin_game);
		}
		
		broadcast_positions = true;
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
		positions[player->GetPid()] = player->player_info->GetPosition();
	}
}

void Game::ProcessInput()
{
	for (auto& player : players)
	{
		auto &client_input = player->client_input;
		if (client_input->requested_move)
		{
			if (current_map->IsCityNeighbour(player->player_info->GetPosition(), client_input->target_city) && player->player_info->GetActions() > 0)
			{
				spdlog::info("a player requested to move");
				broadcast_positions = true;
				player->player_info->SetPosition(client_input->target_city);
				player->player_info->SetActions(player->player_info->GetActions() - 1);
			}
			client_input->requested_move = false;
		}
		if (client_input->client_message != nullptr)
		{
			if (client_input->client_message->first == ClientMessageType::CMESSAGE_CHAT)
			{
				std::string msg = player->player_info->GetName() + std::string(": ") + client_input->client_message->second;
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
