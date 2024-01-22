#include "pch.h"

#include "Connection.h"

#include "Server.h"

Connection::Connection() noexcept :
    IsStopped(true),
    TargetServer(),
    ClientSocket(INVALID_SOCKET)
{

}

Connection::Connection(std::shared_ptr<Server> server) noexcept :
    IsStopped(true),
    TargetServer(server),
    ClientSocket(INVALID_SOCKET)
{

}

Connection::Connection(const Connection& connection) noexcept :
    IsStopped(true),
    TargetServer(connection.TargetServer),
    ClientSocket(INVALID_SOCKET)
{
    UNREFERENCED_PARAMETER(connection);
    Start();
}

Connection::Connection(Connection&& connection) noexcept :
    IsStopped(connection.IsStopped.load()),
    TargetServer(std::move(connection.TargetServer)),
    ClientSocket(connection.ClientSocket),
    Receiver(std::move(connection.Receiver))
{

}

bool Connection::operator==(const Connection& connection) const noexcept
{
    return IsStopped == connection.IsStopped.load();
}

void Connection::Start()
{
    spdlog::info(L"Start connection");
    IsStopped = false;
    SOCKADDR_IN6 clientInfo{};
    int clientInfoSize = sizeof(clientInfo);
    if ((ClientSocket = accept(TargetServer.lock()->GetSocket(), (SOCKADDR*)&clientInfo, &clientInfoSize)) == INVALID_SOCKET)
    {
        spdlog::error(L"accept failed: [{}]", WSAGetLastError());
        return;
    }
    char addressBuffer[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &clientInfo.sin6_addr, addressBuffer, sizeof(addressBuffer));
    spdlog::info("Client connected from: [{}]", addressBuffer);
    Receiver = std::make_unique<std::jthread>(&Connection::Receive, this);
}

void Connection::Receive()
{
    spdlog::info(L"Start receive in this thread: [{}]", std::this_thread::get_id()._Get_underlying_id());
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
        spdlog::info("KYBER KEY {}", mes);//spdlog::to_hex(std::begin(data), std::begin(data) + Capsulator::PublicBytes));
        //spdlog::info("DILITHIUM KEY{}", data);//spdlog::to_hex(std::begin(data) + Capsulator::PublicBytes, std::begin(data) + Signer::PublicBytes));

    }
    spdlog::info(L"Stop receive in this thread: [{}]", std::this_thread::get_id()._Get_underlying_id());
}
