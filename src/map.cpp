#include "map.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include <fstream>
#include <format>
#include <cerrno>
#include <iostream>
#include "city.h"
#include "player_card.h"
#include "infection_card.h"

using json = nlohmann::json;

Map::Map(std::string map_name)
{
	std::string full_path = std::format("assets/maps/{}", map_name);
	std::ifstream f(full_path);
	std::string line;
	while (std::getline(f, line))
	{
		json data = json::parse(line);
		int id = (int)data["_id"];
		std::string name = data["_name"];
		std::vector<int> neighbours = data["_neighbourIds"];
		CityColor city_color = (CityColor)data["_cityColor"];
		PlayerCard player_card = (PlayerCard)data["_playerCard"];
		InfectionCard infection_card = (InfectionCard)data["_infectionCard"];
		if (infection_card >= InfectionCard::NUM_INFECTION_CARDS)
			spdlog::error("Map::Map: invalid InfectionCard");
		if (player_card >= PlayerCard::NUM_PLAYER_CARDS)
			spdlog::error("Map::Map: invalid InfectionCard");
		this->cities[(int)data["_id"]] = std::make_unique<City>(id, name, neighbours, city_color, player_card, infection_card);

	}
}

Map::~Map()
{
}

std::vector<uint8_t> Map::GetResearchStations()
{
	std::vector<uint8_t> list = {};
	for (auto& city : cities)
	{
		if (city.second->HasResearchStation())
		{
			list.push_back(city.first);
		}
	}
	return list;
}

int Map::InfectionCardToCityId(InfectionCard infection_card)
{
	if (infection_card >= InfectionCard::NUM_INFECTION_CARDS)
	{
		spdlog::error("Map::InfectionCardToCityId - invalid InfectionCard {}", (int)infection_card);
		return 0;
	}
		
	for (auto& it : cities)
	{
		if (it.second->GetInfectionCard() == infection_card)
			return it.second->GetId();
	}
	spdlog::error("Map::InfectionCardToCityId - can't find id {}", (int)infection_card);
	return 0;
}

int Map::FindCityId(std::string name)
{
	for (auto& it : cities)
	{
		if (it.second->GetName() == name)
			return it.second->GetId();
	}
	spdlog::error("Map::FindCityId: cant find city {}", name);
	return 0;
}

bool Map::TreatDisease(int city_id, InfectionType type)
{
	if (cities.find(city_id) != cities.end())
	{
		return cities[city_id].get()->TreatDisease(type);
	}
	else
	{
		spdlog::error("GetInfectionCountFromCity could not find city");
		return false;
	}
}

int Map::GetInfectionCountFromCity(int city_id, InfectionType type)
{
	if (cities.find(city_id) != cities.end())
	{
		return cities[city_id].get()->GetInfectionCount(type);
	}
	else
	{
		spdlog::error("GetInfectionCountFromCity could not find city");
		return 0;
	}
}

void Map::ResetExplosions()
{
	for (auto& it: cities) 
	{
		it.second->ResetExplosion();
	}
}

PlayerCard Map::GetPlayerCardFromCityId(int city_id)
{
	if (cities.find(city_id) != cities.end())
	{
		return cities[city_id].get()->GetPlayerCard();
	}
	else
	{
		spdlog::error("GetInfectionCountFromCity could not find city");
		return PlayerCard::NUM_PLAYER_CARDS;
	}
}

bool Map::IsPositionValid(int city_id)
{
	if (cities.find(city_id) != cities.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Map::IsCityNeighbour(int first_id, int second_id)
{
	if (cities.find(first_id) != cities.end() && cities.find(second_id) != cities.end())
	{
		auto& neighbour_ids = cities[first_id].get()->neighbour_ids;
		for (auto it : neighbour_ids)
		{
			if (it == second_id)
			{
				return true;
			}
		}
	}
	return false;
}

bool Map::PlaceResearchStation(int city_id)
{
	if (IsPositionValid(city_id))
	{
		cities[city_id].get()->PlaceResearchStation();
		return true;
	}
	return false;
}

bool Map::ValidateResearchStations(int first_id, int second_id)
{
	if (cities.find(first_id) != cities.end() && cities.find(second_id) != cities.end())
	{
		auto first_research = cities[first_id]->HasResearchStation();
		auto second_research = cities[second_id]->HasResearchStation();
		return first_research && second_research;
	}
	return false;
}

std::pair<int, bool> Map::AddInfection(int id, InfectionType type, uint8_t count)
{
	return cities[id]->AddInfection(type, count);
}

std::vector<int> Map::GetNeighbours(int id)
{
	return cities[id]->neighbour_ids;
}

InfectionType Map::GetInfectionTypeFromCity(int id)
{
	auto& city = cities[id];
	auto color = city->GetColor();
	switch (color)
	{
		case CityColor::CITY_COLOR_BLACK:
		{
			return InfectionType::VIRUS_BLACK;
		}
		case CityColor::CITY_COLOR_BLUE:
		{
			return InfectionType::VIRUS_BLUE;
		}
		case CityColor::CITY_COLOR_RED:
		{
			return InfectionType::VIRUS_RED;
		}
		case CityColor::CITY_COLOR_YELLOW:
		{
			return InfectionType::VIRUS_YELLOW;
		}
		default:
		{
			spdlog::error("Map::GetInfectionTypeFromCity: invalid CityColor");
			return InfectionType::VIRUS_RED;
		}
	}
}
