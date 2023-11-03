#include "map.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"
#include <fstream>
#include <format>
#include <cerrno>
#include <iostream>
#include "city.h"

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
		this->cities[(int)data["_id"]] = std::make_unique<City>(id, name, neighbours);
	}
}

Map::~Map()
{
}

int Map::InfectionCardToCityId(int id)
{
	spdlog::error("Map::InfectionCardToCityId - unimplemented");
	return 0;
}

void Map::ResetExplosions()
{
	for (auto& it: cities) 
	{
		it.second->ResetExplosion();
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
