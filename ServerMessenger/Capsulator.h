#pragma once

struct Capsulator final
{
	struct KyberKeyPair final
	{
		std::vector<uint8_t> PublicKey;
		std::vector<uint8_t> PrivateKey;

		KyberKeyPair() noexcept;
		KyberKeyPair(std::vector<uint8_t>&& publicKey, std::vector<uint8_t>&& privateKey) noexcept;
		KyberKeyPair(KyberKeyPair&& other) noexcept;

		KyberKeyPair& operator=(KyberKeyPair&& other) noexcept;
	};

	struct KyberCapsulated final
	{
		std::vector<uint8_t> CipherText;
		std::vector<uint8_t> SharedKey;

		KyberCapsulated() noexcept;
		KyberCapsulated(std::vector<uint8_t>&& cipherText, std::vector<uint8_t>&& sharedKey) noexcept;
		KyberCapsulated(KyberCapsulated&& other) noexcept;

		KyberCapsulated& operator=(KyberCapsulated&& other) noexcept;
	};

	static KyberKeyPair MakeKyberKeyPair() noexcept;

	static KyberCapsulated KyberEncapsulateKey(const std::vector<uint8_t>& publicKey) noexcept;
	static KyberCapsulated KyberDecapsulateKey(const std::vector<uint8_t>& cipherText, const std::vector<uint8_t>& privateKey) noexcept;

	static bool KyberVerify(const std::vector<uint8_t>& sharedKey1, const std::vector<uint8_t>& sharedKey2) noexcept;

	static size_t PublicBytes;
};
