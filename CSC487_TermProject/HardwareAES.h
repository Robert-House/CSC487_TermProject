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
	unsigned char* GetKey() { return _key; };

private:
	void KeyExpansion(const unsigned char* userkey, unsigned char* key);

	void EncryptAES(const unsigned char* in, unsigned char* out,
		unsigned long length, const unsigned char* key, int numRounds);
	
	void DecryptAES(const unsigned char* in, unsigned char* out,
		unsigned long length, const unsigned char* key, int numRounds);

	__m128i AES_128_ASSIST(__m128i temp1, __m128i temp2);

	unsigned char _key[176];
};