#include "pch.h"

#include "Connection.h"

#include "Server.h"

Connection::Connection() noexcept :
    ClientSocket(INVALID_SOCKET)
{

}

Connection::Connection(const Connection& connection) noexcept
{
    Start();
}

Connection::Connection(Connection&& connection) noexcept :
    IsStopped(connection.IsStopped.load()),
    ClientSocket(connection.ClientSocket)
{
    Receiver.swap(connection.Receiver);
}

bool Connection::operator==(const Connection& connection) const noexcept
{
    return IsStopped == connection.IsStopped.load();
}

void Connection::Start()
{
    spdlog::info(L"Start Connection");
    SOCKADDR_IN6 clientInfo{};
    int clientInfoSize = sizeof(clientInfo);
    if ((ClientSocket = accept(Server::GetSocket(), (SOCKADDR*)&clientInfo, &clientInfoSize)) == INVALID_SOCKET)
    {
        spdlog::error(L"accept failed: [{}]", WSAGetLastError());
        return;
    }
    char addressBuffer[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &clientInfo.sin6_addr, addressBuffer, sizeof(addressBuffer));
    spdlog::info("Client connected from: [{}]", addressBuffer);
    Receiver = std::make_unique<std::jthread>(&Connection::Receive, this);
}

void Connection::Receive() const
{
    spdlog::info(L"Start Receive");
    while (!IsStopped)
    {
        spdlog::info(L">???<");
        int bytesReceived = recv(m_clientSocket, messageInformationBuffer.data(), sizeof(MessageInformation), 0);
        /*std::vector<char> messageInformationBuffer(sizeof(MessageInformation));
        int bytesReceived = recv(m_clientSocket, messageInformationBuffer.data(), sizeof(MessageInformation), 0);
        if (bytesReceived != sizeof(MessageInformation))
        {
            Console::PrintErrorLine(std::format(L"recv messageInformation failed: [{}]", WSAGetLastError()));
            Stop();
            return;
        }
        MessageInformation messageInformation = Message<MessageInformation>::Deserialize(messageInformationBuffer).GetData();
        MessageInformation::MessagesTypes messageType = messageInformation.GetMessageType();
        int messageSize = messageInformation.GetMessageSize();
        std::vector<char> dataBuffer(messageSize);
        bytesReceived = recv(m_clientSocket, dataBuffer.data(), messageSize, 0);
        if (bytesReceived != messageSize)
        {
            Console::PrintErrorLine(std::format(L"recv data failed: [{}] received bytes | [{}] bytes", bytesReceived, messageSize));
            Stop();
            return;
        }
        switch (messageType)
        {
        case MessageInformation::StringMessage:
        {
            Console::PrintLine(std::vformat(L"StringMessage: [{}]", std::make_wformat_args(StringMessage::Deserialize(dataBuffer).GetData())));
            break;
        }
        default:
        {
            Console::PrintErrorLine(std::vformat(L"unknown messageType: [{}]", std::make_wformat_args(static_cast<int>(messageType))));
            Stop();
            return;
        }
        }*/
    }
    spdlog::info(L"Stop Receive");
}

void Connection::Send() const
{
    spdlog::info(L"Start Send");
    while (!IsStopped)
    {
    }
    spdlog::info(L"Stop Send");
}
