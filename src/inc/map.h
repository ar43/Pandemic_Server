#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include "infection_type.h"
#include "infection_card.h"

class City;

class Map
{
public:
	Map(std::string map_name);
	~Map();

	int InfectionCardToCityId(InfectionCard infection_card);
	void ResetExplosions();
	bool IsCityNeighbour(int first_id, int second_id);
	std::pair<int, bool> AddInfection(int id, InfectionType type, uint8_t count);
	std::vector<int> GetNeighbours(int id);
	InfectionType GetInfectionTypeFromCity(int id);
	int FindCityId(std::string name);
private:
	std::unordered_map<int, std::unique_ptr<City>> cities;
};