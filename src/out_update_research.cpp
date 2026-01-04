#include "out_update_research.h"
#include "msg_manager.h"
#include "spdlog/spdlog.h"

OutUpdateResearch::OutUpdateResearch(uint8_t cures, uint8_t num_stations, uint8_t* locations) : OpcodeOut(ServerOpcode::UPDATE_RESEARCH)
{
	this->cures = cures;
	this->num_stations = num_stations;
	this->locations = locations;
}

OutUpdateResearch::~OutUpdateResearch()
{
}

void OutUpdateResearch::WriteBody(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteByte(cures);
	msg_manager->WriteByte(num_stations);
	for (int i = 0; i < num_stations; i++)
	{
		msg_manager->WriteByte(locations[i]);
	}
}
