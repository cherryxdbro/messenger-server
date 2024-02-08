#include "pch.h"

#include "Connection.h"

#include "Server.h"

Connection::Connection() noexcept :
    IsStopped(false),
    TargetServer(nullptr),
    ClientSocket(INVALID_SOCKET),
    Receiver(nullptr)
{

}

Connection::Connection(Server* server) noexcept :
    IsStopped(false),
    TargetServer(server),
    ClientSocket(INVALID_SOCKET),
    Receiver(nullptr)
{

}

Connection::Connection(Connection&& connection) noexcept :
    IsStopped(connection.IsStopped.load()),
    TargetServer(connection.TargetServer),
    ClientSocket(connection.ClientSocket),
    Receiver(connection.Receiver)
{

}

Connection::~Connection() noexcept
{
    if (Receiver)
    {
        if (Receiver->joinable())
        {
            Receiver->join();
        }
    }
}

Connection& Connection::operator=(const Connection& connection) noexcept
{
    IsStopped = connection.IsStopped.load();
    TargetServer = connection.TargetServer;
    ClientSocket = connection.ClientSocket;
    return *this;
}

bool Connection::operator==(const Connection& connection) const noexcept
{
    return this == &connection;
}

void Connection::Start()
{
    spdlog::info(L"start connection");
    IsStopped = false;
    SOCKADDR_IN6 clientInfo{};
    int clientInfoSize = sizeof(clientInfo);
    if ((ClientSocket = accept(TargetServer->GetSocket(), (SOCKADDR*)&clientInfo, &clientInfoSize)) == INVALID_SOCKET)
    {
        spdlog::error(L"accept failed: [{}]", WSAGetLastError());
        return;
    }
    char addressBuffer[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &clientInfo.sin6_addr, addressBuffer, sizeof(addressBuffer));
    spdlog::info("client connected from: [{}]", addressBuffer);
    Receiver = new std::jthread(&Connection::Receive, this);
}

void Connection::Receive()
{
    spdlog::info(L"start receive in this thread: [{}]", std::this_thread::get_id()._Get_underlying_id());
    while (!IsStopped)
    {
        size_t keysSize = Capsulator::PublicBytes + Signer::PublicBytes;
        std::vector<uint8_t> keys(keysSize);
        int bytesReceived = recv(ClientSocket, reinterpret_cast<char*>(keys.data()), keysSize, 0);
        if (bytesReceived <= 0)
        {
            IsStopped = true;
        }
        std::string mes;
        for (uint8_t& i : keys)
        {
            mes += std::to_string(i) + " ";
        }
        spdlog::info("kyber_pk {}", mes);//spdlog::to_hex(std::begin(data), std::begin(data) + Capsulator::PublicBytes));
        //spdlog::info("DILITHIUM KEY{}", data);//spdlog::to_hex(std::begin(data) + Capsulator::PublicBytes, std::begin(data) + Signer::PublicBytes));

    }
    TargetServer->RemoveConnection(*this);
    spdlog::info(L"stop receive in this thread: [{}]", std::this_thread::get_id()._Get_underlying_id());
}
