#include "client.h"
#include "spdlog/spdlog.h"
#include "msg_manager.h"
#include "opcode_manager.h"
#include "utility.h"
#include "client_input.h"

#include "out_message.h"

Client::Client(SOCKET socket, randutils::mt19937_rng *rng)
{
	this->socket = socket;
	this->rng = rng;
	this->state = CSTATE_AWAITING;
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
	if(state == CSTATE_LOBBY || state == CSTATE_GAME)
		spdlog::info("dropped client (pid: {}) (reason: {})",pid, reason);
	else
		spdlog::info("dropped unconnected client (reason: {})", reason);
	//closesocket(socket);
}

void Client::AddToLobby()
{
	//static uint64_t identifier = 0;
	//msg_manager->WriteByte(2);
	//msg_manager->WriteByte(0);
	//msg_manager->WriteByte(0);

	state = CSTATE_LOBBY;
	//msg_manager->InitEncryption(client_session_key, server_session_key);
	//printf("!!!!decryption: %llx %llx\n", client_session_key, server_session_key);
	OutMessage welcome("U are in lobby");
	opcode_manager->Send(welcome);
	timeout_counter = 0;
}

void Client::UpdateAwaiting(uint8_t current_players, uint8_t max_players)
{
	if (state != CSTATE_AWAITING)
		return;
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
		spdlog::info("sent 17");
		msg_manager->WriteByte(17);
		awaiting_substate = 1;
	}
	else if (awaiting_substate == 1)
	{
		if (!msg_manager->PendingInput())
			return;

		if (msg_manager->ReadByte() == 14)
		{
			spdlog::info("received 14");
			if(current_players < max_players)
				AddToLobby();
		}
	}
	
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
	if (state == CSTATE_GAME)
	{
		client_input->Reset();

		opcode_manager->Receive(client_input, pid);

		if (client_input->invalid_opcode)
			return Drop("invalid opcode");

		CheckTimeout();

		if (time == 0)
		{
			OutMessage welcome("U are in game\n");
			opcode_manager->Send(welcome);

			//OpShowInterface design(3559);
			//opcode_manager->Send(design);
		}

		time++;
	}
	else if (state == CSTATE_LOBBY)
	{
		while (msg_manager->PendingInput())
		{
			if (msg_manager->ReadByte() == 255)
				Drop("requested");
			else
				msg_manager->ReadDiscard(1);
		}
		timeout_counter = 0;
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
	char recvbuf[512] = { 0 };
	int iResult = recv(socket, recvbuf, sizeof(recvbuf), 0);
	if (iResult > 512)
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
	char buffer[512];
	int size = (int)output.size();
	int i = 0;
	if (output.size() > 512)
	{
		spdlog::error("SendOutput: too large");
		exit(1);
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
