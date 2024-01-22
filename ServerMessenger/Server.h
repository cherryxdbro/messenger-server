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

	//void RemoveConnection(const Connection& connection);

private:
	void CleanUpServer();

	std::shared_ptr<Server> This;

	u_short Port;
	SOCKET ServerSocket;
	size_t MaxConnections;
	std::atomic_bool IsStopped;
	std::condition_variable ConditionVariable;
	std::mutex ServerMutex;
	std::vector<std::shared_ptr<Connection>> Connections;

	Capsulator::KyberKeyPair KyberKeyPair;
	Signer::DilithiumKeyPair DilithiumKeyPair;
};
