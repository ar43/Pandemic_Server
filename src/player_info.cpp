#include "player_info.h"
#include "utility.h"
#include <string>

PlayerInfo::PlayerInfo()
{
	name = "undefined";
	role = (PlayerRole)0;
	actions = 0;
	position = 0;
}

PlayerInfo::~PlayerInfo()
{
}

void PlayerInfo::SetActions(uint8_t new_actions)
{
	actions = new_actions;
}

uint8_t PlayerInfo::GetActions()
{
	return actions;
}

void PlayerInfo::SetName(std::string new_name)
{
	
	name = new_name;
	std::remove_if(std::begin(name), std::end(name),[](const char & element)
	{
		return util::IsValidCharForName(element);
	});
}

std::string PlayerInfo::GetName()
{
	return name;
}

PlayerRole PlayerInfo::GetRole()
{
	return role;
}

void PlayerInfo::SetPosition(uint8_t new_position)
{
	position = new_position;
}

uint8_t PlayerInfo::GetPosition()
{
	return position;
}

void PlayerInfo::SetRole(PlayerRole new_role)
{
	role = new_role;
}
