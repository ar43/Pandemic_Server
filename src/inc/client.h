#pragma once
#include <winsock2.h>
#include <queue>
#include <string>
#include <assert.h>
#include <memory>
#include "randutils.hpp"
#include "client_state.h"

//class Encryption;

class MsgManager;
class OpcodeManager;
struct ClientInput;

class Client
{
public:
	Client(SOCKET socket);
	~Client();

	std::shared_ptr<MsgManager> msg_manager;
	std::unique_ptr<OpcodeManager> opcode_manager;
	std::shared_ptr<ClientInput> client_input;

	void ReadInput();
	void SendOutput();
	void AddToLobby(int id);

	uint8_t GetPid();
	void SetPid(uint8_t pid);

	int UpdateAwaiting();
	void Update();

	SOCKET socket;

	int awaiting_substate = 0;
	ClientState state;
	bool dropped = false;
	bool disconnected = false;

	uint8_t position = 0;

private:
	static const size_t MAX_PACKET_SIZE = 1024;
	uint64_t time = 0;
	int timeout_counter = 0;
	uint8_t pid;
	randutils::mt19937_rng rng;
	uint32_t current_region = 0;

	std::queue<char> input;
	std::queue<char> output;

	void Drop(std::string reason);
	void RejectConnection(uint8_t code);
	
	void CheckTimeout();
};