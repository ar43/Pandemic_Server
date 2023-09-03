#include "game.h"
#include "spdlog/spdlog.h"
#include "client.h"
#include "client_input.h"
#include "opcode_manager.h"
#include "out_update_players.h"
#include "out_begin_game.h"

Game::Game()
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
		OutUpdatePlayers update_players((uint8_t)players.size(), positions);
		Broadcast(update_players);
		broadcast_positions = false;
	}
}

void Game::Update()
{
	if (!IsInProgress())
		return;

	ProcessInput();
	UpdateGameState();
	BroadcastPositions();

	time++;
}

void Game::Start()
{
	in_progress = true;
	spdlog::info("started game");
	for (int i = 0; i < players.size(); i++)
	{
		auto &player = players.at(i);

		player->state = CSTATE_GAME;
		OutBeginGame out_begin_game((uint8_t)players.size(), player->GetPid());
		player->opcode_manager->Send(out_begin_game);
		broadcast_positions = true;
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
			spdlog::info("a player requested to move");
			broadcast_positions = true;
			player->position = client_input->target_city;
			client_input->requested_move = false;
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
