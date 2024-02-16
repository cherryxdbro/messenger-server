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
    spdlog::info(L"start server in this thread: [{}]", std::this_thread::get_id()._Get_underlying_id());
    IsStopped = false;
    WinsockInitializer winsockInitializer;
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
    {
        SOCKADDR_IN6 localAddress{};
        localAddress.sin6_addr = in6addr_any;
        localAddress.sin6_family = AF_INET6;
        localAddress.sin6_port = htons(Port);
        int addressLength = sizeof(localAddress);
        if (bind(ServerSocket, (SOCKADDR*)&localAddress, addressLength) == SOCKET_ERROR)
        {
            spdlog::error(L"bind failed: [{}]", WSAGetLastError());
            Stop();
            closesocket(ServerSocket);
            return;
        }
        if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR)
        {
            spdlog::error(L"listen failed: [{}]", WSAGetLastError());
            Stop();
            closesocket(ServerSocket);
            return;
        }
        if (getsockname(ServerSocket, (SOCKADDR*)&localAddress, &addressLength) == SOCKET_ERROR)
        {
            spdlog::error(L"getsockname failed: [{}]", WSAGetLastError());
            Stop();
            closesocket(ServerSocket);
            return;
        }
        spdlog::info(L"listening on this port: [{}]", ntohs(localAddress.sin6_port));
    }
    std::jthread removerConnections(&Server::—onnection—leaner, this);
	while (true)
	{
		std::unique_lock<std::mutex> lock(ServerMutex);
        NewConnection.wait(lock, [this]()
            {
                return Connections.size() < MaxConnections || IsStopped;
            });
		if (IsStopped)
		{
            break;
		}
        Connections.emplace_back(Connection(this)).Start();
	}
    closesocket(ServerSocket);
    spdlog::info(L"stop server in this thread: [{}]", std::this_thread::get_id()._Get_underlying_id());
}

void Server::Stop()
{
	IsStopped = true;
    NewConnection.notify_all();
    Connections.clear();
    ConnectionsToRemove.swap(*new std::queue<const Connection*>());
}

SOCKET Server::GetSocket() const
{
    return ServerSocket;
}

void Server::RemoveConnection(const Connection& connection)
{
    if (!IsStopped)
    {
        ConnectionsToRemove.emplace(&connection);
        NewConnection.notify_all();
    }
}

void Server::—onnection—leaner()
{
    spdlog::info(L"start connection cleaner in this thread: [{}]", std::this_thread::get_id()._Get_underlying_id());
    while (true)
    {
        std::unique_lock<std::mutex> lock(ConnectionsMutex);
        NewConnection.wait(lock, [this]()
            {
                return !ConnectionsToRemove.empty() || IsStopped;
            });
        if (IsStopped)
        {
            break;
        }
        while (!ConnectionsToRemove.empty())
        {
            Connections.remove(*ConnectionsToRemove.front());
            ConnectionsToRemove.pop();
        }
    }
    spdlog::info(L"stop connection cleaner in this thread: [{}]", std::this_thread::get_id()._Get_underlying_id());
}
