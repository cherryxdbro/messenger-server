#pragma once

#include "Capsulator.h"
#include "Cryptor.h"
#include "Signer.h"

class Space final
{
public:

private:
	std::string Name;
	std::string Description;
	Capsulator::KyberKeyPair KyberKeyPair;
	Signer::DilithiumKeyPair DilithiumKeyPair;
};
