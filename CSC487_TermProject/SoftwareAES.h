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
	unsigned char* GetKey() { return _key; };

    static const int MAX_DATA_SIZE = 8192; // ~8k

private:
	static const int BLOCK_SIZE = 16;

	void EncryptAES(unsigned char* message, unsigned char* key);
	void KeyExpansion(unsigned char* inputKey, unsigned char* expandedKeys);
	void KeyExpansionCore(unsigned char* in, unsigned char i);

	void SubBytes(unsigned char* state);
	void ShiftRows(unsigned char* state);
	void MixColumns(unsigned char* state);
	void AddRoundKey(unsigned char* state, unsigned char* roundKey);

	void PadMessage(string &message);
	void FillBlock(unsigned char*, unsigned char* block, int index);
	void CopyBack(unsigned char* message, unsigned char* block, int index);

	unsigned char _key[176];
};