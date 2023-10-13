#pragma once
#include <string>
#include <vector>
#include "city_color.h"

class City
{
public:
	City(int id, std::string name, std::vector<int> &neighbour_ids);
	~City();
	std::vector<int> neighbour_ids;
private:
	int id;
	std::string name;
	CityColor city_color;
	
};