#pragma once

#include "Message.h"

struct Signer final
{
	struct DilithiumKeyPair final
	{
		std::vector<uint8_t> PublicKey;
		std::vector<uint8_t> PrivateKey;

		DilithiumKeyPair() noexcept;
		DilithiumKeyPair(std::vector<uint8_t>&& publicKey, std::vector<uint8_t>&& privateKey) noexcept;
		DilithiumKeyPair(DilithiumKeyPair&& other) noexcept;

		DilithiumKeyPair& operator=(DilithiumKeyPair&& other) noexcept;
	};

	static DilithiumKeyPair MakeDilithiumKeyPair() noexcept;

	static Message DilithiumSign(const Message& message, const std::vector<uint8_t>& privateKey) noexcept;
	static Message DilithiumSignOpen(const Message& signedMessage, const std::vector<uint8_t>& publicKey) noexcept;

	static size_t PublicBytes;
};
