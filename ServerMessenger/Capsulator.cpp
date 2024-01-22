#include "pch.h"

#include "Capsulator.h"

extern "C"
{
    #include "kem.h"
}

size_t Capsulator::PublicBytes = CRYPTO_PUBLICKEYBYTES;

Capsulator::KyberKeyPair::KyberKeyPair() noexcept :
    PublicKey(),
    PrivateKey()
{

}

Capsulator::KyberKeyPair::KyberKeyPair(std::vector<uint8_t>&& publicKey, std::vector<uint8_t>&& privateKey) noexcept
{
    PublicKey.swap(publicKey);
    PrivateKey.swap(privateKey);
}

Capsulator::KyberKeyPair::KyberKeyPair(KyberKeyPair&& other) noexcept
{
    PublicKey.swap(other.PublicKey);
    PrivateKey.swap(other.PrivateKey);
}

Capsulator::KyberKeyPair& Capsulator::KyberKeyPair::operator=(KyberKeyPair&& other) noexcept
{
    if (this != &other)
    {
        PublicKey.swap(other.PublicKey);
        PrivateKey.swap(other.PrivateKey);
    }
    return *this;
}

Capsulator::KyberCapsulated::KyberCapsulated() noexcept :
    CipherText(),
    SharedKey()
{

}

Capsulator::KyberCapsulated::KyberCapsulated(std::vector<uint8_t>&& cipherText, std::vector<uint8_t>&& sharedKey) noexcept
{
    CipherText.swap(cipherText);
    SharedKey.swap(sharedKey);
}

Capsulator::KyberCapsulated::KyberCapsulated(KyberCapsulated&& other) noexcept
{
    CipherText.swap(other.CipherText);
    SharedKey.swap(other.SharedKey);
}

Capsulator::KyberCapsulated& Capsulator::KyberCapsulated::operator=(KyberCapsulated&& other) noexcept
{
    if (this != &other)
    {
        CipherText.swap(other.CipherText);
        SharedKey.swap(other.SharedKey);
    }
    return *this;
}

Capsulator::KyberKeyPair Capsulator::MakeKyberKeyPair() noexcept
{
    std::vector<uint8_t> publicKey(CRYPTO_PUBLICKEYBYTES);
    std::vector<uint8_t> privateKey(CRYPTO_SECRETKEYBYTES);
    if (crypto_kem_keypair(publicKey.data(), privateKey.data()))
    {
        return {  };
    }
    return { std::move(publicKey), std::move(privateKey) };
}

Capsulator::KyberCapsulated Capsulator::KyberEncapsulateKey(const std::vector<uint8_t>& publicKey) noexcept
{
    std::vector<uint8_t> cipherText(CRYPTO_CIPHERTEXTBYTES);
    std::vector<uint8_t> sharedKey(CRYPTO_BYTES);
    if (crypto_kem_enc(cipherText.data(), sharedKey.data(), publicKey.data()))
    {
        return {  };
    }
    return { std::move(cipherText), std::move(sharedKey) };
}

Capsulator::KyberCapsulated Capsulator::KyberDecapsulateKey(const std::vector<uint8_t>& cipherText, const std::vector<uint8_t>& privateKey) noexcept
{
    std::vector<uint8_t> cipherTextCopy(CRYPTO_CIPHERTEXTBYTES);
    std::vector<uint8_t> sharedKey(CRYPTO_BYTES);
    memcpy_s(cipherTextCopy.data(), CRYPTO_CIPHERTEXTBYTES, cipherText.data(), CRYPTO_CIPHERTEXTBYTES);
    if (crypto_kem_dec(sharedKey.data(), cipherTextCopy.data(), privateKey.data()))
    {
        return {  };
    }
    return { std::move(cipherTextCopy), std::move(sharedKey) };
}

bool Capsulator::KyberVerify(const std::vector<uint8_t>& sharedKey1, const std::vector<uint8_t>& sharedKey2) noexcept
{
    return memcmp(sharedKey1.data(), sharedKey2.data(), CRYPTO_BYTES) == 0;
}
