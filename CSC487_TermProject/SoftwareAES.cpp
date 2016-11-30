#include "SoftwareAES.h"

SoftwareAES::SoftwareAES()
{

}

SoftwareAES::~SoftwareAES()
{

}

void SoftwareAES::Encrypt(unsigned char * userkey, string message)
{
	unsigned char block[16];
    unsigned char expandedKey[176];

    // Copy string to plaintext block to properly cast to unsigned char
    unsigned char plaintext[MAX_DATA_SIZE];
    strncpy((char*)(plaintext), message.c_str(), sizeof(plaintext));
    plaintext[sizeof(plaintext) - 1] = 0;

    // Do key expansion
    KeyExpansion(userkey, expandedKey);

    // Copy Expanded Key
    for (int i = 0; i < 176; i++)
    {
        _key[i] = expandedKey[i];
    }

    // Chrono Stuff
    chrono::time_point<chrono::system_clock> start, end;

    start = chrono::system_clock::now();
	
    // Run Encryption on blocks
	for (int i = 0; i < message.length(); i += 16)
	{
		FillBlock(plaintext, block, i);
		EncryptAES(block, expandedKey);
		CopyBack(plaintext, block, i);
	}
    end = chrono::system_clock::now();

	cout << endl;
	cout << endl;

    chrono::duration<double, std::micro> elapsed_seconds = end - start;
    time_t end_time = chrono::system_clock::to_time_t(end);
    _time = elapsed_seconds.count();

	//// Print out cipher text to the console
 //   int lineCount = 1;
 //   cout << " " << hex << uppercase << (int)plaintext[0];
	//for (int i = 1; i < message.length(); i++)
	//{
 //       printf(" %02X", plaintext[i]);
 //       lineCount++;
 //       //cout << hex << uppercase << (int)plaintext[i] << " ";
	//	if (lineCount == 16)
	//	{
	//		cout << endl;
 //           lineCount = 0;
	//	}
	//}

    // Print Plaintext
    cout << endl;
}

void SoftwareAES::Decrypt(unsigned char * userkey, string message)
{
}



void SoftwareAES::EncryptAES(unsigned char* message, unsigned char* key)
{
	unsigned char state[BLOCK_SIZE];
	int numOfRounds = 9;

	// Copy key into state
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		state[i] = message[i];
	}

	AddRoundKey(state, key); // Initial Rounds

	// Rounds
	for (int i = 0; i < numOfRounds; i++)
	{
		SubBytes(state);
		ShiftRows(state);
		MixColumns(state);
		AddRoundKey(state, key + (BLOCK_SIZE * (i + 1)));
	}

	// Final Round
	SubBytes(state);
	ShiftRows(state);
	AddRoundKey(state, key + 160);

	// B6 is the first hex
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		message[i] = state[i];
	}
}

void SoftwareAES::KeyExpansion(unsigned char* inputKey, unsigned char* expandedKeys)
{
	int bytesGenerated = 16;
	int rconIteration = 1;
	unsigned char temp[4];

	// First 16 bytes are the original key
	for (int i = 0; i < BLOCK_SIZE; i++)
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

void SoftwareAES::KeyExpansionCore(unsigned char* in, unsigned char i)
{
	// Rotate Left
	unsigned int* q = (unsigned int*)in;
	*q = (*q >> 8) | ((*q & 0xff) << 24);

	// SBOX
	in[0] = sBox[in[0]];
	in[1] = sBox[in[1]];
	in[2] = sBox[in[2]];
	in[3] = sBox[in[3]];

	// RCON
	in[0] ^= rcon[i];
}

void SoftwareAES::SubBytes(unsigned char* state)
{
	// Substitution
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		state[i] = sBox[state[i]];
	}
}

void SoftwareAES::ShiftRows(unsigned char* state)
{
	unsigned char temp[BLOCK_SIZE];

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
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		state[i] = temp[i];
	}
}

void SoftwareAES::MixColumns(unsigned char* state)
{
	unsigned char temp[BLOCK_SIZE];

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
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		state[i] = temp[i];
	}
}

void SoftwareAES::InvMixColumns(unsigned char* state)
{
    unsigned char temp[BLOCK_SIZE];

    // Col 1
    temp[0] = (unsigned char)(GF14[state[0]] ^ GF11[state[1]] ^ GF13[state[2]] ^ GF9[state[3]]);
    temp[1] = (unsigned char)(GF9[state[0]] ^ GF14[state[1]] ^ GF11[state[2]] ^ GF13[state[3]]);
    temp[2] = (unsigned char)(GF13[state[0]] ^ GF9[state[1]] ^ GF14[state[2]] ^ GF11[state[3]]);
    temp[3] = (unsigned char)(GF11[state[0]] ^ GF13[state[1]] ^ GF9[state[2]] ^ GF14[state[3]]);

    // Col 2
    temp[4] = (unsigned char)(GF14[state[4]] ^ GF11[state[5]] ^ GF13[state[6]] ^ GF9[state[7]]);
    temp[5] = (unsigned char)(GF9[state[4]] ^ GF14[state[5]] ^ GF11[state[6]] ^ GF13[state[7]]);
    temp[6] = (unsigned char)(GF13[state[4]] ^ GF9[state[5]] ^ GF14[state[6]] ^ GF11[state[7]]);
    temp[7] = (unsigned char)(GF11[state[4]] ^ GF13[state[5]] ^ GF9[state[6]] ^ GF14[state[7]]);

    // Col 3
    temp[8] = (unsigned char)(GF14[state[8]] ^ GF11[state[9]] ^ GF13[state[10]] ^ GF9[state[11]]);
    temp[9] = (unsigned char)(GF9[state[8]] ^ GF14[state[9]] ^ GF11[state[10]] ^ GF13[state[11]]);
    temp[10] = (unsigned char)(GF13[state[8]] ^ GF9[state[9]] ^ GF14[state[10]] ^ GF11[state[11]]);
    temp[11] = (unsigned char)(GF11[state[8]] ^ GF13[state[9]] ^ GF9[state[10]] ^ GF14[state[11]]);

    // Col 4
    temp[12] = (unsigned char)(GF14[state[12]] ^ GF11[state[13]] ^ GF13[state[14]] ^ GF9[state[15]]);
    temp[13] = (unsigned char)(GF9[state[12]] ^ GF14[state[13]] ^ GF11[state[14]] ^ GF13[state[15]]);
    temp[14] = (unsigned char)(GF13[state[12]] ^ GF9[state[13]] ^ GF14[state[14]] ^ GF11[state[15]]);
    temp[15] = (unsigned char)(GF11[state[12]] ^ GF13[state[13]] ^ GF9[state[14]] ^ GF14[state[15]]);

    // Copy back to state
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        state[i] = temp[i];
    }
}

void SoftwareAES::AddRoundKey(unsigned char* state, unsigned char* roundkey)
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		// XOR
		state[i] ^= roundkey[i];
	}
}

void SoftwareAES::FillBlock(unsigned char* message, unsigned char * block, int index)
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		block[i] = message[i + index];
	}
}

void SoftwareAES::CopyBack(unsigned char* message, unsigned char * block, int index)
{
	for (int i = 0; i < BLOCK_SIZE; i++)
	{
		message[i + index] = block[i];
	}
}
