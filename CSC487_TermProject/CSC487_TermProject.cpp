// CSC487_TermProject.cpp : Defines the entry point for the console application.
//
#pragma once


#include "stdafx.h"
#include <iostream>
#include <string>
#include <random>
#include <time.h>
#include <vector>
#include "HardwareAES.h"
#include "SoftwareAES.h"

using namespace std;

int main()
{
	string message = "This is a message we will encrypt with AES!     ";

	// Control For testing purposes
	unsigned char key[16] =
	{
		1,2,3,4,
		5,6,7,8,
		9,10,11,12,
		13,14,15,16,
	};

	unsigned char* testKey;;
	unsigned char* testKey2;

	unsigned char gKey[16];

	// Seed RNG
	srand(time(NULL));

	// Populate 16-byte key
	for (int i = 0; i < 16; i++)
	{
		gKey[i] = (unsigned char)(rand() & 0x00ff);
	}

	HardwareAES* hwtest = new HardwareAES();
	SoftwareAES* swtest = new SoftwareAES();

	hwtest->Encrypt(key, message);
	swtest->Encrypt(key, message);

	testKey = hwtest->GetKey();
	testKey2 = swtest->GetKey();
	cout << endl;
	cout << endl;
	cout << endl;

	for (int i = 0; i < 176; i++)
	{
		cout << hex << (int)testKey[i] << " " << (int)testKey2[i] << endl;
	}

	return 0;
}

