#include "in_treat_disease.h"
#include "client_input.h"
#include "msg_manager.h"

InTreatDisease::InTreatDisease() : OpcodeIn(ClientOpcode::TREAT_DISEASE)
{

}

InTreatDisease::~InTreatDisease()
{
}

void InTreatDisease::Receive(std::shared_ptr<MsgManager> const& msg_manager, std::shared_ptr<ClientInput> const& client_input)
{
	client_input->treat_disease = msg_manager->ReadByte();
}
