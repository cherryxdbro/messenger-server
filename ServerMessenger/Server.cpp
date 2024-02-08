#include "pch.h"

#include "Server.h"

Server::Server() noexcept :
    Port(5678),
    ServerSocket(INVALID_SOCKET),
    MaxConnections(100),
    IsStopped(true)
{

}

void Server::Start()
{
    spdlog::info(L"start server");
    IsStopped = false;
    WinsockInitializer winsockInitializer;
    {
        if (winsockInitializer.WSAResult != 0)
        {
            spdlog::error(L"wsastartup failed");
            Stop();
            return;
        }
        if ((ServerSocket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
        {
            spdlog::error(L"socket failed: [{}]", WSAGetLastError());
            Stop();
            return;
        }
        SOCKADDR_IN6 localAddress{};
        localAddress.sin6_addr = in6addr_any;
        localAddress.sin6_family = AF_INET6;
        localAddress.sin6_port = htons(Port);
        int addressLength = sizeof(localAddress);
        if (bind(ServerSocket, (SOCKADDR*)&localAddress, addressLength) == SOCKET_ERROR)
        {
            spdlog::error(L"bind failed: [{}]", WSAGetLastError());
            Stop();
            return;
        }
        if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR)
        {
            spdlog::error(L"listen failed: [{}]", WSAGetLastError());
            Stop();
            return;
        }
        if (getsockname(ServerSocket, (SOCKADDR*)&localAddress, &addressLength) == SOCKET_ERROR)
        {
            spdlog::error(L"getsockname failed: [{}]", WSAGetLastError());
            Stop();
            return;
        }
        spdlog::info(L"listening on this port: [{}]", ntohs(localAddress.sin6_port));
    }
    std::jthread removerConnections(&Server::RemoverConnections, this);
	while (!IsStopped)
	{
		std::unique_lock<std::mutex> lock(ServerMutex);
		ConditionVariable.wait(lock, [this]()
            {
                return Connections.size() < MaxConnections || IsStopped;
            });
		if (!IsStopped)
		{
			Connections.emplace_back(Connection(this)).Start();
		}
	}
}

void Server::Stop()
{
    spdlog::info(L"stop server");
	IsStopped = true;
	ConditionVariable.notify_all();
    CleanUpServer();
}

void Server::CleanUpServer()
{
    Connections.clear();
    if (ServerSocket != INVALID_SOCKET)
    {
        closesocket(ServerSocket);
    }
}

SOCKET Server::GetSocket() const
{
    return ServerSocket;
}

void Server::RemoveConnection(const Connection& connection)
{
    std::unique_lock<std::mutex> lock(ConnectionsMutex);
    ConnectionsToRemove.push(&connection);
    if (!IsStopped)
    {
        ConditionVariable.notify_all();
    }
}

void Server::RemoverConnections()
{
    while (!IsStopped)
    {
        if (!IsStopped)
        {
            while (!ConnectionsToRemove.empty())
            {
                Connections.remove(*ConnectionsToRemove.front());
                ConnectionsToRemove.pop();
            }
        }
    }
}
