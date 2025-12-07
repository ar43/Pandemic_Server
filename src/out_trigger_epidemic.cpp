#include "out_trigger_epidemic.h"
#include "msg_manager.h"
#include "spdlog/spdlog.h"

OutTriggerEpidemic::OutTriggerEpidemic(uint8_t pid) : OpcodeOut(ServerOpcode::TRIGGER_EPIDEMIC)
{
	this->pid = pid;
}

OutTriggerEpidemic::~OutTriggerEpidemic()
{
}

void OutTriggerEpidemic::WriteBody(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteByte(pid);
}
