#include "pch.h"

#include "Connection.h"

#include "Server.h"

void Connection::Start()
{
    spdlog::info("Start Connection");
    SOCKADDR_IN6 clientInfo{};
    int clientInfoSize = sizeof(clientInfo);
    SOCKET m_clientSocket;
    if ((m_clientSocket = accept(Server::GetSocket(), (SOCKADDR*)&clientInfo, &clientInfoSize)) == INVALID_SOCKET)
    {
        spdlog::error("accept failed: [{}]", WSAGetLastError());
        return;
    }
    char addressBuffer[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET6, &clientInfo.sin6_addr, addressBuffer, sizeof(addressBuffer));

    int dataLen = static_cast<int>(strlen(addressBuffer));
    int wDataLen = MultiByteToWideChar(CP_UTF8, 0, addressBuffer, dataLen, nullptr, 0);
    std::wstring dataWString(wDataLen, L'\000');
    MultiByteToWideChar(CP_UTF8, 0, addressBuffer, dataLen, &dataWString[0], wDataLen);

    //spdlog::info(std::format("Client connected from: [{}]", std::make_wformat_args(dataWString)));

    while (this)
    {

    }

    spdlog::info("Stop Connection");
}
