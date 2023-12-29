#include "pch.h"

#include "Server.h"

SOCKET Server::m_serverSocket = INVALID_SOCKET;
size_t Server::m_maxConnections = 100;
std::atomic_bool Server::m_isStopped = true;
std::condition_variable Server::m_conditionVariable;
std::mutex Server::m_serverMutex;
std::list<Connection> Server::m_connections;

void Server::Start(u_short port)
{
    spdlog::info(L"Start Server");
    m_isStopped = false;
    WinsockInitializer winsockInitializer;
    {
        if (winsockInitializer.WSAResult != 0)
        {
            spdlog::error(L"WSAStartup failed");
            Stop();
            return;
        }
        if ((m_serverSocket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
        {
            spdlog::error(L"socket failed: [{}]", WSAGetLastError());
            Stop();
            return;
        }
        SOCKADDR_IN6 localAddress{};
        localAddress.sin6_addr = in6addr_any;
        localAddress.sin6_family = AF_INET6;
        localAddress.sin6_port = htons(port);
        int addressLength = sizeof(localAddress);
        if (bind(m_serverSocket, (SOCKADDR*)&localAddress, addressLength) == SOCKET_ERROR)
        {
            spdlog::error(L"bind failed: [{}]", WSAGetLastError());
            Stop();
            return;
        }
        if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
        {
            spdlog::error(L"listen failed: [{}]", WSAGetLastError());
            Stop();
            return;
        }
        if (getsockname(m_serverSocket, (SOCKADDR*)&localAddress, &addressLength) == SOCKET_ERROR)
        {
            spdlog::error(L"getsockname failed: [{}]", WSAGetLastError());
            Stop();
            return;
        }
        spdlog::info(L"Listening on this port: [{}]", ntohs(localAddress.sin6_port));
    }
	while (!m_isStopped)
	{
		std::unique_lock<std::mutex> lock(m_serverMutex);
		m_conditionVariable.wait(lock, []()
            {
                return m_connections.size() < m_maxConnections || m_isStopped;
            });
		if (!m_isStopped)
		{
			m_connections.emplace_back(Connection()).Start();
		}
	}
}

void Server::Stop()
{
    spdlog::info(L"Stop Server");
	m_isStopped = true;
	m_conditionVariable.notify_all();
    CleanUpServer();
}

void Server::CleanUpServer()
{
    m_connections.clear();
    if (m_serverSocket != INVALID_SOCKET)
    {
        closesocket(m_serverSocket);
    }
}

SOCKET Server::GetSocket()
{
    return m_serverSocket;
}

void Server::RemoveConnection(const Connection& connection)
{
    m_connections.remove(connection);
    if (!m_isStopped)
    {
        m_conditionVariable.notify_all();
    }
}
