// CSC487_TermProject.cpp : Defines the entry point for the console application.
//
#pragma once


#include "stdafx.h"
#include <iostream>
#include <string>
#include "HardwareAES.h"
#include "SoftwareAES.h"

using namespace std;

int main()
{
	string message = "This is a message we will encrypt with AES!";

	// TODO: Intel DRNG
	unsigned char key[16] =
	{
		1,2,3,4,
		5,6,7,8,
		9,10,11,12,
		13,14,15,16,
	};

	HardwareAES* hwtest = new HardwareAES();
	SoftwareAES* swtest = new SoftwareAES();

	//hwtest->Encrypt(key, message);
	swtest->Encrypt(key, message);

	return 0;
}