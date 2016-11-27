#pragma once

#include <iostream>
#include <string>
#include "LookupTables.h"

using namespace std;

class SoftwareAES
{
public:
	SoftwareAES();
	~SoftwareAES();

	void Encrypt(unsigned char* userkey, string message);
	void Decrypt(unsigned char* userkey, string message);

private:
	static const int NUM_BYTES = 16;

	void EncryptAES(unsigned char* message, unsigned char* key);
	void KeyExpansion(unsigned char* inputKey, unsigned char* expandedKeys);
	void KeyExpansionCore(unsigned char* in, unsigned char i);

	void SubBytes(unsigned char* state);
	void ShiftRows(unsigned char* state);
	void MixColumns(unsigned char* state);
	void AddRoundKey(unsigned char* state, unsigned char* roundKey);

	void PadMessage(string &message);
};