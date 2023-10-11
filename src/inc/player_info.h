#pragma once
#include <string>
#include <memory>
#include <player_role.h>

class CardStack;

class PlayerInfo
{
public:
	PlayerInfo();
	~PlayerInfo();

	void SetActions(uint8_t new_actions);
	uint8_t GetActions();

	void SetName(std::string new_name);
	std::string GetName();

	void SetRole(PlayerRole new_role);
	PlayerRole GetRole();

	void SetPosition(uint8_t new_position);
	uint8_t GetPosition();

	std::unique_ptr<CardStack> hand;

private:
	std::string name;
	PlayerRole role;
	uint8_t actions;
	uint8_t position;
};