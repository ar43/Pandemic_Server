#pragma once

enum class ClientOpcode
{
	IDLE,
	MOVE,
	CLIENT_MESSAGE,
	READY,
	DISCARD
};