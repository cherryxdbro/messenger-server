#include "pch.h"

#include "Signer.h"

extern "C"
{
    #include <sign.h>
}

size_t Signer::PublicBytes = CRYPTO_PUBLICKEYBYTES;

Signer::DilithiumKeyPair::DilithiumKeyPair() noexcept
{

}

Signer::DilithiumKeyPair::DilithiumKeyPair(std::vector<uint8_t>&& publicKey, std::vector<uint8_t>&& privateKey) noexcept
{
    PublicKey.swap(publicKey);
    PrivateKey.swap(privateKey);
}

Signer::DilithiumKeyPair::DilithiumKeyPair(DilithiumKeyPair&& other) noexcept
{
    PublicKey.swap(other.PublicKey);
    PrivateKey.swap(other.PrivateKey);
}

Signer::DilithiumKeyPair& Signer::DilithiumKeyPair::operator=(DilithiumKeyPair&& other) noexcept
{
    PublicKey.swap(other.PublicKey);
    PrivateKey.swap(other.PrivateKey);
    return *this;
}

Signer::DilithiumKeyPair Signer::MakeDilithiumKeyPair() noexcept
{
    std::vector<uint8_t> publicKey(CRYPTO_PUBLICKEYBYTES);
    std::vector<uint8_t> privateKey(CRYPTO_SECRETKEYBYTES);
    if (crypto_sign_keypair(publicKey.data(), privateKey.data()) != 0)
    {
        return {  };
    }
    return { std::move(publicKey), std::move(privateKey) };
}

Message Signer::DilithiumSign(const Message& message, const std::vector<uint8_t>& privateKey) noexcept
{
    size_t signedMessageLength;
    std::vector<uint8_t> signedMessage(message.Data.size() + CRYPTO_BYTES);
    if (crypto_sign(signedMessage.data(), &signedMessageLength, message.Data.data(), message.Data.size(), privateKey.data()) != 0)
    {
        return {  };
    }
    return { std::move(signedMessage) };
}

Message Signer::DilithiumSignOpen(const Message& signedMessage, const std::vector<uint8_t>& publicKey) noexcept
{
    size_t messageLength;
    std::vector<uint8_t> message(signedMessage.Data.size());
    if (crypto_sign_open(message.data(), &messageLength, signedMessage.Data.data(), signedMessage.Data.size(), publicKey.data()) != 0)
    {
        return {  };
    }
    return { std::move(message) };
}
