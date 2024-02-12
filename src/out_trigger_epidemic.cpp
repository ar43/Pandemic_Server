#include "out_trigger_epidemic.h"
#include "msg_manager.h"
#include "spdlog/spdlog.h"

OutTriggerEpidemic::OutTriggerEpidemic(int temp) : OpcodeOut(ServerOpcode::TRIGGER_EPIDEMIC)
{
	this->temp = temp;
}

OutTriggerEpidemic::~OutTriggerEpidemic()
{
}

void OutTriggerEpidemic::Send(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteOpcode(GetId());
}
