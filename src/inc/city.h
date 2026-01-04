#pragma once
#include <string>
#include <vector>
#include <utility>
#include "city_color.h"
#include "player_card.h"
#include "infection_card.h"
#include <infection_type.h>

constexpr auto NUM_INFECTIONS = 4;

class City
{
public:
	City(int id, std::string name, std::vector<int> &neighbour_ids, CityColor city_color, PlayerCard player_card, InfectionCard infection_card);
	~City();
	std::vector<int> neighbour_ids;
	std::pair<int, bool> AddInfection(InfectionType type, uint8_t count);
	void ResetExplosion();
	CityColor GetColor();
	PlayerCard GetPlayerCard();
	bool HasResearchStation();
	void PlaceResearchStation();
	InfectionCard GetInfectionCard();
	int GetInfectionCount(InfectionType type);
	bool TreatDisease(InfectionType type);
	int GetId();
	std::string GetName();
private:
	int id;
	std::string name;
	CityColor city_color;
	PlayerCard player_card;
	InfectionCard infection_card;

	bool can_explode;
	uint8_t infections[NUM_INFECTIONS];

	bool research_station;
};