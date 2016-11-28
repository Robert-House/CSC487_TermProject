#pragma once

#include <string>
//-------------------------------------------------------------------------//
// File  :  HardwareAES.h                                                  //
// Author:  Robert House 
//          28 NOV 2016                                                    //
//-------------------------------------------------------------------------//
// HardwareAES: [SUMMATION OF CLASS FUNCTIONALITY]                        //
//                                                                         //
//	 -- [HIGHLIGHT OF FUNCTIONALITY 1]                                     //
//	 -- [HIGHLIGHT OF FUNCTIONALITY 2]                                     //
//	 -- [HIGHLIGHT OF FUNCTIONALITY 3]                                     //
//                                                                         //
// Assumptions:                                                            //
//   -- >> [ASSUMPTION 1]                                                  //
//-------------------------------------------------------------------------//

#include <iostream>
#include "wmmintrin.h"

using namespace std;

class HardwareAES
{
public:
	HardwareAES();
	~HardwareAES();

    //------------------------------[Encrypt]-------------------------------------
    // Takes input key and input plaintext to transform into ciphertext
    //
    // Inputs:    userkey -- 128-bit encryption key
	void Encrypt(unsigned char* userkey, string message);
	void Decrypt(unsigned char* userkey, string ciphertext);
	unsigned char* GetKey() { return _key; };

    static const int MAX_DATA_SIZE = 8192; // ~8k

private:
	void KeyExpansion(const unsigned char* userkey, unsigned char* key);

	void EncryptAES(const unsigned char* in, unsigned char* out,
		unsigned long length, const unsigned char* key, int numRounds);
	
	void DecryptAES(const unsigned char* in, unsigned char* out,
		unsigned long length, const unsigned char* key, int numRounds);

	__m128i AES_128_ASSIST(__m128i temp1, __m128i temp2);

	unsigned char _key[176];
};