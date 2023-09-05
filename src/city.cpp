#include "city.h"

City::City(int id, std::string name, std::vector<int>& neighbour_ids)
{
	this->id = id;
	this->name = name;
	for (auto neigbour_id : neighbour_ids)
	{
		this->neighbour_ids.push_back(neigbour_id);
	}
}

City::~City()
{
}
