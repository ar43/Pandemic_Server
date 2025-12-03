#include "out_trigger_infection.h"
#include "msg_manager.h"
#include "spdlog/spdlog.h"

OutTriggerInfection::OutTriggerInfection(uint8_t card_id, std::vector<std::pair<uint8_t, uint8_t>>& infection_data) : infection_data(infection_data), OpcodeOut(ServerOpcode::TRIGGER_INFECTION)
{
	this->card_id = card_id;
}

OutTriggerInfection::~OutTriggerInfection()
{
}

void OutTriggerInfection::WriteBody(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteByte(card_id);

	auto length = infection_data.size();
	if (length > UINT8_MAX)
		spdlog::error("OutTriggerInfection::Send - size overflow");
	msg_manager->WriteByte((uint8_t)length);

	for (size_t i = 0; i < length; i++)
	{
		uint8_t infection_type = infection_data.at(i).first;
		uint8_t city_id = infection_data.at(i).second;
		msg_manager->WriteByte(infection_type);
		msg_manager->WriteByte(city_id);
	}
}
