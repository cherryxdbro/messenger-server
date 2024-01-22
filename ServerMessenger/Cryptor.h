#pragma once

#include "Message.h"

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

constexpr auto CRYPTO_BYTES = 32;

struct Cryptor final
{
	static Message ToMessage(const std::wstring& message) noexcept;
	static std::wstring ToWString(const Message& message) noexcept;

	static Message Encrypt(const Message& message, const std::vector<uint8_t>& sharedKey, const std::vector<uint8_t>& initializationVector) noexcept;
	static Message Decrypt(const Message& message, const std::vector<uint8_t>& sharedKey, const std::vector<uint8_t>& initializationVector) noexcept;
};
