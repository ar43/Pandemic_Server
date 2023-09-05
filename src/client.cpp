#include "client.h"
#include "spdlog/spdlog.h"
#include "msg_manager.h"
#include "opcode_manager.h"
#include "utility.h"
#include "client_input.h"

#include "out_server_message.h"

Client::Client(SOCKET socket, randutils::mt19937_rng *rng)
{
	this->socket = socket;
	this->rng = rng;
	this->state = ClientState::CSTATE_AWAITING;
	msg_manager = std::make_shared<MsgManager>(&this->input,&this->output);
	opcode_manager = std::make_unique<OpcodeManager>(msg_manager);
	client_input = std::make_shared<ClientInput>();
}

Client::~Client()
{
	spdlog::debug("client destructor called");
}

void Client::Drop(std::string reason)
{
	dropped = true;
	OutServerMessage msg(ServerMessageType::SMESSAGE_INFO, std::format("Dropped. Reason: {}",reason));
	opcode_manager->Send(msg);
	if(state == ClientState::CSTATE_LOBBY || state == ClientState::CSTATE_GAME)
		spdlog::info("dropped client (pid: {}) (reason: {})",pid, reason);
	else
		spdlog::info("dropped unconnected client (reason: {})", reason);
	//closesocket(socket);
}

void Client::AddToLobby(int id)
{
	state = ClientState::CSTATE_LOBBY;
	msg_manager->WriteByte(0);
	std::string lobby_msg = std::string("You are in lobby(") + std::to_string(id) + std::string(")");
	OutServerMessage welcome(ServerMessageType::SMESSAGE_INFO, lobby_msg);
	opcode_manager->Send(welcome);
	timeout_counter = 0;
}

int Client::UpdateAwaiting()
{
	if (state != ClientState::CSTATE_AWAITING)
		return -1;
	/*
	if (timeout_counter >= 10)
	{
		Drop("logout timeout");
		return;
	}
	*/

	timeout_counter++;
	
	if (awaiting_substate == 0)
	{
		spdlog::info("sent 11");
		msg_manager->WriteByte(11);
		awaiting_substate = 1;
	}
	else if (awaiting_substate == 1)
	{
		if (!msg_manager->PendingInput())
			return -1;

		if (msg_manager->ReadByte() == 12)
		{
			if (!msg_manager->PendingInput())
			{
				Drop("invalid handshake - expected lobby id");
				return -1;
			}
			
			return (int)msg_manager->ReadByte();
		}
	}
	else if (awaiting_substate == 2) //game full
	{
		msg_manager->WriteByte(1);
		awaiting_substate = 1;
	}
	else if (awaiting_substate == 3) //game not found
	{
		msg_manager->WriteByte(2);
		awaiting_substate = 1;
	}
	return -1;
}

void Client::CheckTimeout()
{
	if (client_input->num_actions == 0)
	{
		timeout_counter++;
		if (timeout_counter > 100)
		{
			Drop("timeout");
			return;
		}
	}
	else
	{
		timeout_counter = 0;
	}
}

void Client::Update()
{
	if (state == ClientState::CSTATE_GAME)
	{
		client_input->Reset();

		auto success = opcode_manager->Receive(client_input, pid);
		if (!success)
		{
			return Drop("opcode error");
		}

		CheckTimeout();

		if (time == 0)
		{
			OutServerMessage welcome(ServerMessageType::SMESSAGE_INFO, "U are in game");
			opcode_manager->Send(welcome);

			//OpShowInterface design(3559);
			//opcode_manager->Send(design);
		}

		time++;
	}
	else if (state == ClientState::CSTATE_LOBBY)
	{
		client_input->Reset();
		while (msg_manager->PendingInput())
		{
			client_input->num_actions = 1;
			if (msg_manager->ReadByte() == 255)
				Drop("requested");
			else
				msg_manager->ReadDiscard(1);
		}
		CheckTimeout();
	}
}

void Client::RejectConnection(uint8_t code)
{
	//msg_manager->WriteByte(code);
	//SendOutput();
	Drop("rejected connection with code " + std::to_string(code));
	//active = false;
	//closesocket(socket);
}

void Client::ReadInput()
{
	if (dropped || disconnected)
		return;
	char recvbuf[Client::MAX_PACKET_SIZE] = { 0 };
	int iResult = recv(socket, recvbuf, sizeof(recvbuf), 0);
	if (iResult > Client::MAX_PACKET_SIZE)
	{
		spdlog::error("ReadInput: number of bytes received is too large ({})", iResult);
		//closesocket(it->socket);
	}
	else if (iResult > 0)
	{
		if (!input.empty())
			spdlog::warn("Received new buffer before previous was empty? Unexpected?");
		//spdlog::debug("ReadInput: number of bytes received: {}", iResult);
		//printf("Byte string:");
		for (int i = 0; i < iResult; i++)
		{
			//printf(" %d", recvbuf[i]);
			input.push(recvbuf[i]);
		}
		//printf("\n");


	}
	else if (iResult < 0)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return;
		}
		spdlog::warn("ReadInput: recv failed with error: {}", WSAGetLastError());
		//closesocket(socket);
	}
	else if (iResult == 0)
	{
		spdlog::info("ReadInput: client disconnected...");
		disconnected = true;
		//closesocket(socket);
	}
}

void Client::SendOutput()
{
	if (dropped || disconnected || output.empty())
		return;
	char buffer[Client::MAX_PACKET_SIZE];
	int size = (int)output.size();
	int i = 0;
	if (output.size() > Client::MAX_PACKET_SIZE)
	{
		spdlog::error("SendOutput: too large");
	}
	while (!output.empty())
	{
		buffer[i] = output.front();
		output.pop();
		i++;
	}
	int iSendResult = send( socket, buffer, size, 0 );
	if (iSendResult == SOCKET_ERROR) 
	{
		spdlog::warn("SendOutput: send failed with error: {}", WSAGetLastError());
		//closesocket(socket);
	}
	spdlog::debug("SendOutput: bytes sent: {}", iSendResult);
}

uint8_t Client::GetPid()
{
	return pid;
}

void Client::SetPid(uint8_t pid)
{
	this->pid = pid;
}
