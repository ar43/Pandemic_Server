#pragma once
#include <unordered_map>
#include <string>
#include <memory>

class City;

class Map
{
public:
	Map(std::string map_name);
	~Map();

	bool IsCityNeighbour(int first_id, int second_id);
private:
	std::unordered_map<int, std::unique_ptr<City>> cities;
};