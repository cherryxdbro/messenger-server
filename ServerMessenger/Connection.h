#pragma once

class Connection final
{
public:
	Connection() noexcept;
	Connection(const Connection& connection) noexcept;
	Connection(Connection&& connection) noexcept;

	bool operator==(const Connection& connection) const noexcept;

	void Start();
	void Receive() const;
	void Send() const;

	std::atomic_bool IsStopped = false;
	SOCKET ClientSocket;
	std::unique_ptr<std::jthread> Receiver;
};
