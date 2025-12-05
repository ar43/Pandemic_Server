#pragma once

enum class ClientOpcode
{
	IDLE,
	MOVE,
	CLIENT_MESSAGE,
	READY,
	DISCARD,
	JOIN_LOBBY,
	TREAT_DISEASE
};