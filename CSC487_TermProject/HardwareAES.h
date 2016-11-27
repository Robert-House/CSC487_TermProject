#pragma once

#include <string>
#include <iostream>
#include "wmmintrin.h"

using namespace std;

class HardwareAES
{
public:
	HardwareAES();
	~HardwareAES();

	void Encrypt(unsigned char* userkey, string message);
	void Decrypt();

private:
	void KeyExpansion(const unsigned char* userkey, unsigned char* key);
	void EncryptAES(const unsigned char* in, unsigned char* out,
		unsigned long length, const char* key, int numRounds);
	void DecryptAES(const unsigned char* in, unsigned char* out,
		unsigned long length, const char* key, int numRounds);

	unsigned char ciphertext[2048];
	unsigned char plaintext[2048];
};