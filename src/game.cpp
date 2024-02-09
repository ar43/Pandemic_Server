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
#include "timer.h"
#include "card_stack.h"
#include "player_card.h"
#include "out_update_player_card.h"
#include "infection_card.h"
#include "out_trigger_infection.h"

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
	game_begin_timer = std::make_unique<Timer>();
	player_card_deck = std::make_unique<CardStack>((uint8_t)(PlayerCard::NUM_PLAYER_CARDS));
	infection_card_deck = std::make_unique<CardStack>((uint8_t)(InfectionCard::NUM_INFECTION_CARDS));
	infection_card_discard_pile = std::make_unique<CardStack>(0);
	lobby_player_count_timer = std::make_unique <Timer>();
	lobby_player_count_timer->Start(1000.0, true);
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

void Game::SendLobbyPlayerCount()
{
	OutServerMessage msg(ServerMessageType::SMESSAGE_LOBBY, std::format("Players in lobby: {}/{}", players.size(),GetMaxPlayers()));
	Broadcast(msg);
}

void Game::Update()
{
	ProcessClientMessages();

	if (!IsInProgress())
	{
		if (game_begin_timer->Tick())
			Start();
		if (lobby_player_count_timer->Tick())
			SendLobbyPlayerCount();
	}
	else
	{
		if (players.size() != max_players)
		{
			Kill("player disconnected");
			return;
		}

		ProcessInput();
		UpdateGameState();
		BroadcastPositions();

		ticks++;
	}
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

void Game::Start()
{
	if (players.size() != GetMaxPlayers())
	{
		OutServerMessage cancel_msg(ServerMessageType::SMESSAGE_INFO, "Game start canceled because a player left.");
		Broadcast(cancel_msg);
		return;
	}
	in_progress = true;
	LoadMap("map_default.json");
	spdlog::info("started game with id {}", GetId());
	GenerateRoles();
	ValidateNames();
	std::vector<std::string> player_names;
	std::vector<PlayerRole> player_roles;
	for (auto& player : players)
	{
		player->player_info->SetActions(99);
		player->player_info->SetPosition(current_map->FindCityId("Atlanta"));
		player->state = ClientState::CSTATE_GAME;
		player_names.push_back(player->player_info->GetName());
		player_roles.push_back(player->player_info->GetRole());

		player->player_info->hand->AddCard(player_card_deck->Draw());
		player->player_info->hand->AddCard(player_card_deck->Draw());
	}
	for (auto& player : players)
	{
		OutBeginGame out_begin_game((uint8_t)players.size(), player->GetPid(),&player_names,&player_roles);
		player->opcode_manager->Send(out_begin_game);

		OutUpdatePlayerCard out_update_player_card(player->GetPid(), false, (uint8_t)player->player_info->hand->GetSize(), player->player_info->hand->GetPointer());
		Broadcast(out_update_player_card);
	}

	DrawInfectionCard(3);
	DrawInfectionCard(3);
	DrawInfectionCard(3);
	DrawInfectionCard(2);
	DrawInfectionCard(2);
	DrawInfectionCard(2);
	DrawInfectionCard(1);
	DrawInfectionCard(1);
	DrawInfectionCard(1);

	OutServerMessage start_game_msg(ServerMessageType::SMESSAGE_INFO, "Game started.");
	Broadcast(start_game_msg);

	broadcast_positions = true;
}

void Game::OnPlayerJoin(std::string player_name)
{
	//send "player blabla has joined lobby(id)"
	OutServerMessage joined_msg(ServerMessageType::SMESSAGE_INFO, std::format("Player {} has joined Lobby({}).", player_name, std::to_string(id)));
	Broadcast(joined_msg);
	if (players.size() == max_players)
	{
		OutServerMessage start_msg(ServerMessageType::SMESSAGE_INFO, "Game starting in 3 seconds...");
		Broadcast(start_msg);
		game_begin_timer->Start(3000.0,false);
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

void Game::ProcessClientMessages()
{
	for (auto& player : players)
	{
		auto &client_input = player->client_input;
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

void Game::DebugInfect(InfectionCard target_card)
{
	std::vector<std::pair<uint8_t, uint8_t>> infection_data;
	auto card = (uint8_t)target_card;
	auto city_id = current_map->InfectionCardToCityId((InfectionCard)card);
	spdlog::debug("debugging city_id: {}", city_id);
	InfectionType type = current_map->GetInfectionTypeFromCity(city_id);

	current_map->ResetExplosions();

	InfectCity(city_id, card, type, 1, infection_data);

	OutTriggerInfection packet(card, infection_data);
	Broadcast(packet);
}

void Game::DrawInfectionCard(uint8_t infection_multiplier)
{
	if (infection_multiplier <= 0 || infection_multiplier > 3)
	{
		spdlog::error("Game::DrawInfectionCard: infection_multiplier invalid");
		return;
	}

	std::vector<std::pair<uint8_t, uint8_t>> infection_data;
	auto card = infection_card_deck->Draw();
	auto city_id = current_map->InfectionCardToCityId((InfectionCard)card);
	InfectionType type = current_map->GetInfectionTypeFromCity(city_id);

	current_map->ResetExplosions();

	InfectCity(city_id, card, type, infection_multiplier, infection_data);

	infection_card_discard_pile->AddCard(card);

	OutTriggerInfection packet(card, infection_data);
	Broadcast(packet);
}

void Game::InfectCity(int city_id, uint8_t card_id, InfectionType type, uint8_t infection_multiplier, std::vector<std::pair<uint8_t, uint8_t>> &infection_data)
{
	auto data = current_map->AddInfection(city_id, type, infection_multiplier);
	bool explosion = data.second;
	int new_infection_count = data.first;
	for (int i = 0; i < new_infection_count; i++)
	{
		infection_data.push_back(std::make_pair((uint8_t)type, (uint8_t)city_id));
	}
	if (explosion)
	{
		infection_data.push_back(std::make_pair((uint8_t)InfectionType::EXPLOSION, (uint8_t)city_id));
		auto neighbours = current_map->GetNeighbours(city_id);
		for (auto cid : neighbours)
		{
			InfectCity(cid, card_id, type, infection_multiplier, infection_data);
		}
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
