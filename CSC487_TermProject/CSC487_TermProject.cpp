// CSC487_TermProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include "wmmintrin.h"
#include "LookupTables.h"

using namespace std;

static const int NUM_BYTES = 16;

void EncryptAES(unsigned char* message, unsigned char* key);
void KeyExpansion(unsigned char* inputKey, unsigned char* expandedKeys);
void KeyExpansionCore(unsigned char* in, unsigned char i);

void SubBytes(unsigned char* state);
void ShiftRows(unsigned char* state);
void MixColumns(unsigned char* state);
void AddRoundKey(unsigned char* state, unsigned char* roundKey);

void PadMessage(string &message);

int main()
{
	string message = "This is a message we will encrypt with AES!";

	unsigned char key[NUM_BYTES] =
	{
		1,2,3,4,
		5,6,7,8,
		9,10,11,12,
		13,14,15,16,
	};

	PadMessage(message);

	//char* msg = new char(message.length() + 1);
	//strcpy(msg, message.c_str());
	unsigned char msg[] = "This is a message we will encrypt with AES!     ";

	for (int i = 0; i < message.size(); i += 16)
	{
		EncryptAES(msg + i, key);
	}

	for (int i = 0; i < message.length(); i++)
	{
		cout << hex << (unsigned int)msg[i] << endl;
	}

	return 0;
}

void EncryptAES(unsigned char* message, unsigned char* key)
{
	unsigned char state[NUM_BYTES];
	unsigned char expandedKey[176];
	int numOfRounds = 9;

	// Copy key into state
	for (int i = 0; i < NUM_BYTES; i++)
	{
		state[i] = message[i];
	}

	KeyExpansion(key, expandedKey); // TODO: Move this outside Encrypt!!!!!!
	AddRoundKey(state, key); // Initial Rounds

	// Rounds
	for (int i = 0; i < numOfRounds; i++)
	{
		SubBytes(state);
		ShiftRows(state);
		MixColumns(state);
		AddRoundKey(state, expandedKey + (NUM_BYTES * (i + 1)));
	}

	// Final Round
	SubBytes(state);
	ShiftRows(state);
	AddRoundKey(state, expandedKey + 160);

	// B6 is the first hex
	for (int i = 0; i < NUM_BYTES; i++)
	{
		message[i] = state[i];
	}
}

void KeyExpansion(unsigned char inputKey[16], unsigned char expandedKeys[176])
{
	int bytesGenerated = 16;
	int rconIteration = 1;
	unsigned char temp[4];
	
	// First 16 bytes are the original key
	for (int i = 0; i < NUM_BYTES; i++)
	{
		expandedKeys[i] = inputKey[i];
	}

	while (bytesGenerated < 176)
	{
		// Read last 4 bytes into the temp array
		for (int i = 0; i < 4; i++)
		{
			temp[i] = expandedKeys[i + bytesGenerated - 4];
		}

		// Grab the next 16 bytes
		if (bytesGenerated % 16 == 0)
		{
			KeyExpansionCore(temp, rconIteration);
			rconIteration++;
		}

		// XOR
		for (unsigned char a = 0; a < 4; a++)
		{
			expandedKeys[bytesGenerated] = expandedKeys[bytesGenerated - 16] ^ temp[a];
			bytesGenerated++;
		}
	}
}

void KeyExpansionCore(unsigned char* in, unsigned char i)
{
	// Rotate Left
	unsigned int* q = (unsigned int*)in;
	*q = (*q >> 8) | ((*q & 0xff) << 24);

	/*unsigned char t = in[0];
	in[0] = in[1];
	in[1] = in[2];
	in[2] = in[3];
	in[3] = t;*/

	// SBOX
	in[0] = s[in[0]];
	in[1] = s[in[1]];
	in[2] = s[in[2]];
	in[3] = s[in[3]];

	// RCON
	in[0] ^= rcon[i];
}

void SubBytes(unsigned char* state)
{
	// Substitution
	for (int i = 0; i < NUM_BYTES; i++)
	{
		state[i] = s[state[i]]; // TODO: Replace s with sBox
	}
}

void ShiftRows(unsigned char* state)
{
	unsigned char temp[NUM_BYTES];

	// Col 1
	temp[0] = state[0];
	temp[1] = state[5];
	temp[2] = state[10];
	temp[3] = state[15];

	// Col 2
	temp[4] = state[4];
	temp[5] = state[9];
	temp[6] = state[14];
	temp[7] = state[3];

	// Col 3
	temp[8] = state[8];
	temp[9] = state[13];
	temp[10] = state[2];
	temp[11] = state[7];

	// Col 4
	temp[12] = state[12];
	temp[13] = state[1];
	temp[14] = state[6];
	temp[15] = state[11];

	// Copy back to state
	for (int i = 0; i < NUM_BYTES; i++)
	{
		state[i] = temp[i];
	}
}

void MixColumns(unsigned char* state)
{
	unsigned char temp[NUM_BYTES];

	// Col 1
	temp[0] = (unsigned char)(GF2[state[0]] ^ GF3[state[1]] ^ state[2] ^ state[3]);
	temp[1] = (unsigned char)(state[0] ^ GF2[state[1]] ^ GF3[state[2]] ^ state[3]);
	temp[2] = (unsigned char)(state[0] ^ state[1] ^ GF2[state[2]] ^ GF3[state[3]]);
	temp[3] = (unsigned char)(GF3[state[0]] ^ state[1] ^ state[2] ^ GF2[state[3]]);

	// Col 2
	temp[4] = (unsigned char)(GF2[state[4]] ^ GF3[state[5]] ^ state[6] ^ state[7]);
	temp[5] = (unsigned char)(state[4] ^ GF2[state[5]] ^ GF3[state[6]] ^ state[7]);
	temp[6] = (unsigned char)(state[4] ^ state[5] ^ GF2[state[6]] ^ GF3[state[7]]);
	temp[7] = (unsigned char)(GF3[state[4]] ^ state[5] ^ state[6] ^ GF2[state[7]]);

	// Col 3
	temp[8] = (unsigned char)(GF2[state[8]] ^ GF3[state[9]] ^ state[10] ^ state[11]);
	temp[9] = (unsigned char)(state[8] ^ GF2[state[9]] ^ GF3[state[10]] ^ state[11]);
	temp[10] = (unsigned char)(state[8] ^ state[9] ^ GF2[state[10]] ^ GF3[state[11]]);
	temp[11] = (unsigned char)(GF3[state[8]] ^ state[9] ^ state[10] ^ GF2[state[11]]);

	// Col 4
	temp[12] = (unsigned char)(GF2[state[12]] ^ GF3[state[13]] ^ state[14] ^ state[15]);
	temp[13] = (unsigned char)(state[12] ^ GF2[state[13]] ^ GF3[state[14]] ^ state[15]);
	temp[14] = (unsigned char)(state[12] ^ state[13] ^ GF2[state[14]] ^ GF3[state[15]]);
	temp[15] = (unsigned char)(GF3[state[12]] ^ state[13] ^ state[14] ^ GF2[state[15]]);

	// Copy back to state
	for (int i = 0; i < NUM_BYTES; i++)
	{
		state[i] = temp[i];
	}
}

void AddRoundKey(unsigned char* state, unsigned char* roundkey)
{
	for (int i = 0; i < NUM_BYTES; i++)
	{
		// XOR
		state[i] ^= roundkey[i];
	}
}

void PadMessage(string &message)
{
	int mod = message.length() % 16;

	if (mod != 0)
	{
		// Pad Zeroes
		for (int i = 16 - mod; i > 0; i--)
		{
			message = message + " ";
		}
	}
}