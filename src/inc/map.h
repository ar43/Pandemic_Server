#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "infection_type.h"

class City;

class Map
{
public:
	Map(std::string map_name);
	~Map();

	int InfectionCardToCityId(int id);
	void ResetExplosions();
	bool IsCityNeighbour(int first_id, int second_id);
	std::pair<int, bool> AddInfection(int id, InfectionType type, uint8_t count);
	std::vector<int> GetNeighbours(int id);
	InfectionType GetInfectionTypeFromCity(int id);
private:
	std::unordered_map<int, std::unique_ptr<City>> cities;
};