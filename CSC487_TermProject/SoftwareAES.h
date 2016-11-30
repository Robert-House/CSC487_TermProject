#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <time.h>
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

    static const int MAX_DATA_SIZE = 8192; // ~16MB
    double _time = 0;

private:
	static const int BLOCK_SIZE = 16;

	void EncryptAES(unsigned char* message, unsigned char* key);
	void KeyExpansion(unsigned char* inputKey, unsigned char* expandedKeys);
	void KeyExpansionCore(unsigned char* in, unsigned char i);
    void InverseKeyExpansion(unsigned char* in, unsigned char* out);

	void SubBytes(unsigned char* state);
	void ShiftRows(unsigned char* state);
	void MixColumns(unsigned char* state);
    void InvMixColumns(unsigned char* state);
	void AddRoundKey(unsigned char* state, unsigned char* roundKey);

	void FillBlock(unsigned char*, unsigned char* block, int index);
	void CopyBack(unsigned char* message, unsigned char* block, int index);

	unsigned char _key[176];
};