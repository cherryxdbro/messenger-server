#pragma once

class Connection final
{
public:
	void Start();

	bool operator==(const Connection& connection) const = default;
};
