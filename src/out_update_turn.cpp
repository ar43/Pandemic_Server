#include "out_update_turn.h"
#include "msg_manager.h"
#include "spdlog/spdlog.h"

OutUpdateTurn::OutUpdateTurn(TurnUpdateType turn_update_type, uint8_t pid, uint8_t actions) : OpcodeOut(ServerOpcode::UPDATE_TURN)
{
	this->turn_update_type = turn_update_type;
	this->pid = pid;
	this->actions = actions;
}

OutUpdateTurn::~OutUpdateTurn()
{
}

void OutUpdateTurn::WriteBody(std::shared_ptr<MsgManager> const& msg_manager)
{
	msg_manager->WriteByte((uint8_t)turn_update_type);
	msg_manager->WriteByte(pid);
	msg_manager->WriteByte(actions);
}
