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
	std::ofstream output("test.txt");
	std::ifstream f(full_path);
	std::string line;
	while (std::getline(f, line))
	{
		json data = json::parse(line);
		int id = (int)data["_id"];
		std::string name = data["_name"];
		std::vector<int> neighbours = data["_neighbourIds"];
		this->cities[(int)data["_id"]] = std::make_unique<City>(id, name, neighbours);
	}
	
	
}

Map::~Map()
{

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
