#pragma once

class Server;

class Connection final
{
public:
	Connection() noexcept;
	Connection(std::shared_ptr<Server> server) noexcept;
	Connection(const Connection& connection) noexcept;
	Connection(Connection&& connection) noexcept;

	bool operator==(const Connection& connection) const noexcept;

	void Start();
	void Receive();

	std::atomic_bool IsStopped;
	std::weak_ptr<Server> TargetServer;
	SOCKET ClientSocket;
	std::unique_ptr<std::jthread> Receiver;
};
