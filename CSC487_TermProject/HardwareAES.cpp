//-------------------------------------------------------------------------//
// HardwareAES.cpp                                                         //
// Author:  Robert House 
//          28 NOV 2016                                                    //
//-------------------------------------------------------------------------//
// [CLASS NAME]: [SUMMATION OF CLASS FUNCTIONALITY]                        //
//                                                                         //
//	 -- [HIGHLIGHT OF FUNCTIONALITY 1]                                     //
//	 -- [HIGHLIGHT OF FUNCTIONALITY 2]                                     //
//	 -- [HIGHLIGHT OF FUNCTIONALITY 3]                                     //
//                                                                         //
// Assumptions:                                                            //
//   -- >> [ASSUMPTION 1]                                                  //
//-------------------------------------------------------------------------//

#include "HardwareAES.h"

HardwareAES::HardwareAES()
{
	// Nothing here for now
}

HardwareAES::~HardwareAES()
{
	// Nothing to destruct
}

//------------------------------[Copy]-------------------------------------
// [DESCRIPTION]
//
// Inputs:    [INPUT 1] -- [DESCRIPTION]
void HardwareAES::Encrypt(unsigned char* userkey, string message)
{
	int numR = 10;
	unsigned char key[176];

    // Chrono Stuff
    chrono::time_point<chrono::system_clock> start, end;

    // Do some string copying stuff because I am stupid
    unsigned char plaintext[MAX_DATA_SIZE];
    unsigned char ciphertext[MAX_DATA_SIZE];
    unsigned char dPlainText[MAX_DATA_SIZE];

    strncpy((char*)(plaintext), message.c_str(), sizeof(plaintext));
    plaintext[sizeof(plaintext) - 1] = 0;
	
	KeyExpansion(userkey, key);

    // Measure Time
    start = chrono::system_clock::now();
	EncryptAES(plaintext, ciphertext, message.length(), key, numR);
    end = chrono::system_clock::now();

    // Print out cipher text to the console
    int lineCount = 1;
    cout << " " << hex << uppercase << (int)ciphertext[0];
    for (int i = 1; i < message.length(); i++)
    {
        printf(" %02X", ciphertext[i]);
        lineCount++;
        //cout << hex << uppercase << (int)plaintext[i] << " ";
        if (lineCount == 64)
        {
            cout << endl;
            lineCount = 0;
        }
    }

	for (int i = 0; i < 176; i++)
	{
		_key[i] = key[i];
	}

	cout << endl;

	DecryptAES(ciphertext, dPlainText, message.length(), key, numR);

	for (int i = 0; i < message.length(); i++)
	{
		cout << dPlainText[i] << " ";
	}

    cout << endl;
    chrono::duration<double, std::micro> elapsed_seconds = end - start;
    time_t end_time = chrono::system_clock::to_time_t(end);
    //cout << elapsed_seconds.count();

    _time = elapsed_seconds.count();
}

void HardwareAES::Decrypt(unsigned char* userkey, string ciphertext)
{

}

void HardwareAES::KeyExpansion(const unsigned char* userkey, unsigned char* key)
{
	__m128i temp1, temp2;
	__m128i *Key_Schedule = (__m128i*)key;     
	
	temp1 = _mm_loadu_si128((__m128i*)userkey);  
	Key_Schedule[0] = temp1;     
	
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x1);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[1] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1, 0x2);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[2] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1, 0x4);     
	temp1 = AES_128_ASSIST(temp1, temp2);     
	Key_Schedule[3] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1, 0x8);     
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[4] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1, 0x10);     
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[5] = temp1;

	temp2 = _mm_aeskeygenassist_si128(temp1, 0x20);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[6] = temp1;
	
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x40);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[7] = temp1;
	
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x80);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[8] = temp1;
	
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x1b);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[9] = temp1;
	
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x36);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[10] = temp1;
}

void HardwareAES::EncryptAES(const unsigned char* in, unsigned char* out,
	unsigned long length, const unsigned char* key, int numRounds)
{
	__m128i temp;
	__m128i tKey;
	int i;
	int j;

	// Divide data into 16 byte chunks
	if (length % 16)
	{
		length = length / 16 + 1;
	}
	else
	{
		length = length / 16;
	}

	// Encrypt
	for (i = 0; i < length; i++)
	{
		// Round 1
		temp = _mm_loadu_si128(&((__m128i*)in)[i]);
		tKey = _mm_loadu_si128(&((__m128i*)key)[0]);

		// DO XOR
		temp = _mm_xor_si128(temp, tKey);

		// Rounds 2-9
		for (j = 1; j < numRounds; j++)
		{
			tKey = _mm_loadu_si128(&((__m128i*)key)[j]);
			temp = _mm_aesenc_si128(temp, tKey);
		}

		// Last round
		tKey = _mm_loadu_si128(&((__m128i*)key)[j]);
		temp = _mm_aesenclast_si128(temp, tKey);

        // Write ciphertext
		_mm_storeu_si128(&((__m128i*)out)[i], temp);
	}
}

void HardwareAES::DecryptAES(const unsigned char* in, unsigned char* out,
	unsigned long length, const unsigned char* key, int numRounds)
{
	__m128i temp;
	__m128i tkey;
	int i;
	int j;
	// Divide data into 16 byte chunks
	if (length % 16)
	{
		length = length / 16 + 1;
	}
	else
	{
		length = length / 16;
	}

	for (i = 0; i < length; i++) 
	{ 
		// Load block
		temp = _mm_loadu_si128(&((__m128i*)in)[i]);
		
		// decryptKey[0] = encryptKey[numRounds]
		tkey = _mm_loadu_si128(&((__m128i*)key)[numRounds]);
		
		// Add round key
		temp = _mm_xor_si128(temp, tkey);

		// for decryptRound 2-9
		for (j = numRounds - 1; j > 0; j--)
		{
			tkey = _mm_loadu_si128(&((__m128i*)key)[j]);
			tkey = _mm_aesimc_si128(tkey); // INVERSE COLUMNS!!!!!!
			temp = _mm_aesdec_si128(temp, tkey);
		}
		
		// Round 10 decryptKey[numRounds] = encryptKey[0]
		tkey = _mm_loadu_si128(&((__m128i*)key)[0]);
		temp = _mm_aesdeclast_si128(temp, tkey);
		
		// Write out block. PHEW, DONE!
		_mm_storeu_si128(&((__m128i*)out)[i], temp);
	}
}


__m128i HardwareAES::AES_128_ASSIST(__m128i temp1, __m128i temp2)
{
    __m128i temp3;
    temp2 = _mm_shuffle_epi32(temp2, 0xff);
    temp3 = _mm_slli_si128(temp1, 0x4);
    temp1 = _mm_xor_si128(temp1, temp3);
    temp3 = _mm_slli_si128(temp3, 0x4);
    temp1 = _mm_xor_si128(temp1, temp3);
    temp3 = _mm_slli_si128(temp3, 0x4);
    temp1 = _mm_xor_si128(temp1, temp3);
    temp1 = _mm_xor_si128(temp1, temp2);
    return temp1;
}