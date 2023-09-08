#include "opcode_manager.h"
#include "spdlog/spdlog.h"
#include "client_input.h"
#include "msg_manager.h"
#include "opcode_out.h"
#include "opcode_in.h"
#include "in_idle.h"
#include "in_error.h"
#include "in_move.h"
#include "in_client_message.h"

OpcodeManager::OpcodeManager(std::shared_ptr<MsgManager> msg_manager)
{
	this->msg_manager = msg_manager;
}

OpcodeManager::~OpcodeManager() = default;

void OpcodeManager::Send(OpcodeOut & opcode)
{
	opcode.Send(msg_manager);
}

bool OpcodeManager::Receive(std::shared_ptr<ClientInput> const& client_input, uint8_t pid)
{
	while (msg_manager->PendingInput())
	{
		auto opcode_id = msg_manager->ReadOpcode();
		auto opcode = GetOpcode(opcode_id);
		opcode->Receive(msg_manager,client_input);
		if(opcode_id != 0)
			spdlog::info("client({}): opcode {} - pnum: {}", pid, opcode_id, client_input->num_actions);
		if (client_input->invalid_opcode)
		{
			spdlog::warn("client opcode {} - invalid opcode", opcode_id);
			return false;
		}
		if (msg_manager->GetError())
		{
			spdlog::warn("client opcode {} - malformed packet", opcode_id);
			return false;
		}
			
		client_input->num_actions++;
	}
	return true;
}

std::unique_ptr<OpcodeIn> OpcodeManager::GetOpcode(uint8_t id)
{
	switch (id)
	{
		case 0: return std::make_unique<InIdle>();
		case 1: return std::make_unique<InMove>();
		case 2: return std::make_unique<InClientMessage>();
		default: return std::make_unique<InError>();

	}
}
