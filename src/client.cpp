#include "client.h"
#include "spdlog/spdlog.h"
#include "msg_manager.h"
#include "opcode_manager.h"
#include "utility.h"
#include "client_input.h"
#include "timer.h"

#include "out_server_message.h"
#include "player_info.h"



Client::Client(SOCKET socket)
{
	this->socket = socket;
	this->state = ClientState::CSTATE_AWAITING;
	msg_manager = std::make_shared<MsgManager>(&this->input,&this->output);
	opcode_manager = std::make_unique<OpcodeManager>(msg_manager);
	client_input = std::make_shared<ClientInput>();
	player_info = std::make_unique<PlayerInfo>();
	timeout_timer = std::make_unique<Timer>();
}

Client::~Client()
{
	spdlog::debug("client destructor called");
}

#pragma warning( push )
#pragma warning( disable : 4244)
void Client::PrintSocketError()
{
	wchar_t *s = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&s, 0, NULL);
	//fprintf(stderr, "%S\n", s);
	std::wstring ws(s);
	// your new String
	std::string str(ws.begin(), ws.end());
	spdlog::warn(str);
	LocalFree(s);
}
#pragma warning( pop ) 

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
	timeout_timer->Start(TIMEOUT_TIME,false);
}

int Client::UpdateAwaiting()
{
	if (state != ClientState::CSTATE_AWAITING)
		return -1;
	
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
			auto lobby_id = (int)msg_manager->ReadByte();
			auto name_length = msg_manager->ReadByte();
			if (name_length > Client::MAX_NAME_LEN || name_length < 3)
			{
				Drop("invalid name length");
				return -1;
			}
			std::string name = msg_manager->ReadString(name_length);
			player_info->SetName(name);
			if (player_info->GetName().length() < 3)
			{
				Drop("invalid name");
				return -1;
			}
			if (msg_manager->GetError())
			{
				Drop("error during handshake");
				return -1;
			}
			return lobby_id;
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
		if (timeout_timer->Tick())
		{
			Drop("timeout");
			return;
		}
	}
	else
	{
		timeout_timer->Restart();
	}
}

void Client::Update()
{
	client_input->Reset();

	auto success = opcode_manager->Receive(client_input, pid);
	if (!success)
	{
		return Drop("opcode error");
	}

	CheckTimeout();

	ticks++;
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
	const int MAX_INPUT_PER_TICK = 5120;
	char recvbuf[Client::PACKET_SIZE] = { 0 };
	if (!input.empty())
		spdlog::warn("Receiving new buffer before all data is processed? Could be an error.");
	int total_bytes_received = 0;
	while (true)
	{
		int result = recv(socket, recvbuf, sizeof(recvbuf), 0);
		if (result > 0)
		{
			for (int i = 0; i < result; i++)
			{
				input.push(recvbuf[i]);
				total_bytes_received++;
				if (total_bytes_received > MAX_INPUT_PER_TICK)
				{
					//100% trying to flood the server, get rid of him
					spdlog::warn("Detected packet flood");
					Drop("packet flood");
					return;
				}
			}
		}
		else if (result < 0)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				break;
			}
			if (WSAGetLastError() == WSAECONNRESET)
			{
				spdlog::info("ReadInput: client disconnected...");
				disconnected = true;
				break;
			}
			spdlog::warn("ReadInput: recv failed with error: {}", WSAGetLastError());
			Client::PrintSocketError();
			break;
			//closesocket(socket);
		}
		else if (result == 0)
		{
			spdlog::info("ReadInput: client disconnected...");
			disconnected = true;
			break;
			//closesocket(socket);
		}
	}
}

void Client::SendOutput()
{
	if (disconnected || output.empty())
		return;
	char buffer[Client::PACKET_SIZE];
	int i = 0;
	while (!output.empty())
	{
		if (i == sizeof(buffer))
		{
			int send_result = send( socket, buffer, i, 0 );
			if (send_result == SOCKET_ERROR) 
			{
				spdlog::warn("SendOutput: send failed with error: {}", WSAGetLastError());
				Client::PrintSocketError();
				return;
			}
			memset(buffer, 0, sizeof(buffer));
			i = 0;
		}
		buffer[i] = output.front();
		output.pop();
		i++;
	}
	int send_result = send( socket, buffer, i, 0 );
	if (send_result == SOCKET_ERROR) 
	{
		spdlog::warn("SendOutput: send failed with error: {}", WSAGetLastError());
		Client::PrintSocketError();
		return;
	}
	spdlog::debug("SendOutput: bytes sent: {}", send_result);
}

const uint8_t Client::GetPid()
{
	return pid;
}

void Client::SetPid(uint8_t pid)
{
	this->pid = pid;
}
