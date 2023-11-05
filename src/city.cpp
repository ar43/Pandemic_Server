#include "city.h"
#include "spdlog/spdlog.h"

City::City(int id, std::string name, std::vector<int>& neighbour_ids, CityColor city_color, PlayerCard player_card, InfectionCard infection_card)
{
	this->id = id;
	this->name = name;
	for (auto neigbour_id : neighbour_ids)
	{
		this->neighbour_ids.push_back(neigbour_id);
	}

	this->can_explode = true;
	for (int i = 0; i < NUM_INFECTIONS; i++)
		this->infections[i] = 0;
	
	this->player_card = player_card;
	this->infection_card = infection_card;
	this->city_color = city_color;
}

City::~City()
{
}

std::pair<int, bool> City::AddInfection(InfectionType type, uint8_t count)
{
	size_t index = (size_t)type;
	if (index >= NUM_INFECTIONS || index < 0)
	{
		spdlog::error("City::AddInfection: type out of bounds");
		return std::make_pair(0, false);
	}

	if (infections[index] == 3)
	{
		if (can_explode)
		{
			can_explode = false;
			return std::make_pair(0, true);
		}
		else
		{
			return std::make_pair(0, false);
		}
	}

	uint8_t old_infections = infections[index];
	infections[index] += count;
	if (infections[index] > 3)
	{
		infections[index] = 3;
	}

	return std::make_pair(infections[index]-old_infections, false);
}

void City::ResetExplosion()
{
	can_explode = true;
}

CityColor City::GetColor()
{
	return city_color;
}

PlayerCard City::GetPlayerCard()
{
	return player_card;
}

InfectionCard City::GetInfectionCard()
{
	return infection_card;
}

int City::GetId()
{
	return id;
}
