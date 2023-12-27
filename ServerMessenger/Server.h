#pragma once

#include "Connection.h"
#include "WinsockInitializer.h"

class Server final
{
public:
	static void Start(u_short port);
	static void Stop();

	static SOCKET GetSocket();

	static void RemoveConnection(const Connection& connection);

private:
	static void CleanUpServer();

	static SOCKET m_serverSocket;
	static size_t m_maxConnections;
	static std::atomic_bool m_isStopped;
	static std::condition_variable m_conditionVariable;
	static std::mutex m_serverMutex;
	static std::list<Connection> m_connections;
};
