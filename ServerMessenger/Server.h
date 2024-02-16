#pragma once

#include "Connection.h"
#include "WinsockInitializer.h"

#include "Capsulator.h"
#include "Cryptor.h"
#include "Signer.h"

class Server final
{
public:
	Server() noexcept;

	void Start();
	void Stop();

	SOCKET GetSocket() const;

	void RemoveConnection(const Connection& connection);
	void —onnection—leaner();

private:
	std::atomic_bool IsStopped;
	u_short Port;
	SOCKET ServerSocket;
	size_t MaxConnections;
	std::condition_variable NewConnection;
	std::mutex ServerMutex;
	std::mutex ConnectionsMutex;
	std::list<Connection> Connections;
	std::queue<const Connection*> ConnectionsToRemove;

	Capsulator::KyberKeyPair KyberKeyPair;
	Signer::DilithiumKeyPair DilithiumKeyPair;
};
