#include "game.h"
#include "spdlog/spdlog.h"
#include "client.h"
#include "client_input.h"
#include "opcode_manager.h"
#include "out_update_players.h"

Game::Game()
{
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
	OutUpdatePlayers update_players((uint8_t)players.size(), positions);
	Broadcast(update_players);
}

void Game::Start()
{
	in_progress = true;
	for (int i = 0; i < players.size(); i++)
	{
		auto &player = players.at(i);

		player->state = CSTATE_GAME;
	}
}

void Game::UpdateGameState()
{
	for (int i = 0; i < players.size(); i++)
	{
		auto &player = players.at(i);
		
		positions[player->GetPid()] = player->position;
	}
}

void Game::ProcessInput()
{
	for (int i = 0; i < players.size(); i++)
	{
		auto &player = players.at(i);
		auto &client_input = players.at(i)->client_input;
		if (client_input->requested_move)
		{
			player->position = client_input->target_city;
		}
	}
}

void Game::Broadcast(OpcodeOut& opcode)
{
	for (int i = 0; i < players.size(); i++)
	{
		auto &player = players.at(i);
		
		player->opcode_manager->Send(opcode);
	}
}
