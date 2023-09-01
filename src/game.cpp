#include "game.h"
#include "spdlog/spdlog.h"
#include "client.h"
#include "client_input.h"
#include "opcode_manager.h"
#include "out_update_players.h"

Game::Game(std::vector<std::shared_ptr<Client>>* players)
{
	this->players = players;
	if (this->players == NULL)
		spdlog::error("game has null players function");
}

bool Game::IsInProgress()
{
	return in_progress;
}

void Game::Update()
{
	if (!IsInProgress())
		return;
	ProcessInput();
	UpdateGameState();
}

void Game::Start()
{
	in_progress = true;
	for (int i = 0; i < players->size(); i++)
	{
		auto &player = players->at(i);

		player->state = CSTATE_GAME;
	}
}

void Game::UpdateGameState()
{
	for (int i = 0; i < players->size(); i++)
	{
		auto &player = players->at(i);
		
		positions[player->GetPid()] = player->position;
	}
}

void Game::ProcessInput()
{
	for (int i = 0; i < players->size(); i++)
	{
		auto &player = players->at(i);
		auto &client_input = players->at(i)->client_input;
		if (client_input->requested_move)
		{
			player->position = client_input->target_city;
		}
	}
}

void Game::Broadcast(OpcodeOut& opcode)
{
	for (int i = 0; i < players->size(); i++)
	{
		auto &player = players->at(i);
		OutUpdatePlayers update_players((uint8_t)players->size(), positions);
		player->opcode_manager->Send(update_players);
	}
}
