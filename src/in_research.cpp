#include "in_research.h"
#include "client_input.h"
#include "msg_manager.h"

InResearch::InResearch() : OpcodeIn(ClientOpcode::RESEARCH)
{

}

InResearch::~InResearch()
{
}

void InResearch::Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input)
{
	client_input->place_research_station = (bool)msg_manager->ReadByte();
	client_input->create_cure = (bool)msg_manager->ReadByte();

	if (client_input->place_research_station)
		client_input->create_cure = false;
}
