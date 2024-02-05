#pragma once

class Server;

class Connection final
{
public:
	Connection() noexcept;
	Connection(Server* server) noexcept;
	Connection(Connection&& connection) noexcept;

	~Connection()
	{
		IsStopped = false;
	}

	Connection& operator=(const Connection& connection) noexcept;
	bool operator==(const Connection& connection) const noexcept;

	void Start();

private:
	void Receive();

	std::atomic_bool IsStopped;
	Server* TargetServer;
	SOCKET ClientSocket;
	std::unique_ptr<std::jthread> Receiver;
};
