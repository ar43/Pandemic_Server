#include "out_treat_disease.h"
#include "msg_manager.h"

OutTreatDisease::OutTreatDisease(uint8_t pid, uint8_t type, uint8_t new_count) : OpcodeOut(ServerOpcode::TREAT_DISEASE)
{
	this->pid = pid;
	this->type = type;
	this->new_count = new_count;
}

OutTreatDisease::~OutTreatDisease()
{
}

void OutTreatDisease::WriteBody(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteByte(pid);
	msg_manager->WriteByte(type);
	msg_manager->WriteByte(new_count);
}
