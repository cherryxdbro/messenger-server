#include "pch.h"

#include "Server.h"

SOCKET Server::m_serverSocket = INVALID_SOCKET;
size_t Server::m_maxConnections = 3;
std::atomic_bool Server::m_isStopped = true;
std::condition_variable Server::m_conditionVariable;
std::mutex Server::m_serverMutex;
std::list<Connection> Server::m_connections;

void Server::Start(u_short port)
{
    spdlog::info("Start Server");
    m_isStopped = false;
    WinsockInitializer winsockInitializer;
    {
        if (winsockInitializer.WSAResult != 0)
        {
            spdlog::error("WSAStartup failed");
            Stop();
            return;
        }
        if ((m_serverSocket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
        {
            spdlog::error("socket failed: [{}]", WSAGetLastError());
            Stop();
            return;
        }
        SOCKADDR_IN6 localAddress{};
        localAddress.sin6_addr = in6addr_any;
        localAddress.sin6_family = AF_INET6;
        localAddress.sin6_port = htons(port);//static_cast<unsigned short>(std::stoi(Console::ReadLine(L"Select the port [enter 0 for automatic assignment]: "))));
        int addressLength = sizeof(localAddress);
        if (bind(m_serverSocket, (SOCKADDR*)&localAddress, addressLength) == SOCKET_ERROR)
        {
            spdlog::error("bind failed: [{}]", WSAGetLastError());
            Stop();
            return;
        }
        if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
        {
            spdlog::error("listen failed: [{}]", WSAGetLastError());
            Stop();
            return;
        }
        std::unique_ptr<IP_ADAPTER_ADDRESSES> adapterAddresses;
        ULONG outputBufferLength{};
        ULONG family = localAddress.sin6_family;
        ULONG result;
        result = GetAdaptersAddresses(family, GAA_FLAG_INCLUDE_PREFIX, nullptr, nullptr, &outputBufferLength);
        if (result == ERROR_BUFFER_OVERFLOW)
        {
            adapterAddresses.reset(new IP_ADAPTER_ADDRESSES[outputBufferLength]);
        }
        result = GetAdaptersAddresses(family, GAA_FLAG_INCLUDE_PREFIX, nullptr, adapterAddresses.get(), &outputBufferLength);
        if (result != NO_ERROR)
        {
            spdlog::error("GetAdaptersAddresses failed : [{}] ", result);
            Stop();
            return;
        }
        for (PIP_ADAPTER_ADDRESSES pCurrAddresses = adapterAddresses.get(); pCurrAddresses; pCurrAddresses = pCurrAddresses->Next)
        {
            for (PIP_ADAPTER_UNICAST_ADDRESS pUnicast = pCurrAddresses->FirstUnicastAddress; pUnicast; pUnicast = pUnicast->Next)
            {
                SOCKADDR_IN6* address = reinterpret_cast<SOCKADDR_IN6*>(pUnicast->Address.lpSockaddr);
                char addressBuffer[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &address->sin6_addr, addressBuffer, INET6_ADDRSTRLEN);
                int dataLen = static_cast<int>(wcslen(pCurrAddresses->FriendlyName));
                int mDataLen = WideCharToMultiByte(CP_UTF8, 0, pCurrAddresses->FriendlyName, dataLen, nullptr, 0, nullptr, nullptr);
                std::string dataString(mDataLen, '\0');
                WideCharToMultiByte(CP_UTF8, 0, pCurrAddresses->FriendlyName, dataLen, &dataString[0], mDataLen, nullptr, nullptr);
                spdlog::info("You may be able to connect to the server through this adapter: [{}] at this IP: [{}]", dataString, std::string(addressBuffer));
            }
        }
        if (getsockname(m_serverSocket, (SOCKADDR*)&localAddress, &addressLength) == SOCKET_ERROR)
        {
            spdlog::error("getsockname failed: [{}]", WSAGetLastError());
            Stop();
            return;
        }
        spdlog::info("Listening on this port: [{}]", ntohs(localAddress.sin6_port));
    }
	while (!m_isStopped)
	{
		std::unique_lock<std::mutex> lock(m_serverMutex);
		m_conditionVariable.wait(lock, []() { return m_connections.size() < m_maxConnections || m_isStopped; });
		if (!m_isStopped)
		{
			m_connections.emplace_back(Connection()).Start();
		}
	}
}

void Server::Stop()
{
    spdlog::info("Stop Server");
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
