#pragma once
#include <string>
#include <vector>
#include <utility>
#include "city_color.h"
#include <infection_type.h>

constexpr auto NUM_INFECTIONS = 4;

class City
{
public:
	City(int id, std::string name, std::vector<int> &neighbour_ids);
	~City();
	std::vector<int> neighbour_ids;
	std::pair<int, bool> AddInfection(InfectionType type, uint8_t count);
	void ResetExplosion();
	CityColor GetColor();
private:
	int id;
	std::string name;
	CityColor city_color;

	bool can_explode;
	uint8_t infections[NUM_INFECTIONS];
};