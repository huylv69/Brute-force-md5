/* 
 * File:   MD5.cpp
 * Author: HuyLV
 *
 */

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <map>
#include <list>
#include <iterator>
#include <openssl/md5.h>

using namespace std;

char alphabet[26] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

// Convert orders of password to String
void convertIndexToString(int indexPass, int len, char convertIndexToString[]);

// Encode String to MD5
void encodeMD5(const char *string, char *md5Encode);

// Find Md5
void findPass(int begin, int end, char *md5Encode, int len, int rank);

// Processing on each rank
void processOnRank(char *md5Encode, int numberProcess, int passwordLength, int rank);

int main(int argc, char *argv[])
{
	int size, rank;
	MPI_Init(&argc, &argv);
	char md5Encode[33];
	int passwordLength;
	if (argc != 3)
		return -1;
	sscanf(argv[1], "%s", md5Encode);		// MD5 input
	sscanf(argv[2], "%d", &passwordLength); // Length of origin password
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	processOnRank(md5Encode, size, passwordLength, rank);
	MPI_Finalize();
	return 0;
}

void convertIndexToString(int indexPass, int len, char convertIndexToString[])
{
	int pos = len - 1;
	int index;
	int j;
	for (j = 0; j < len; j++)
	{
		convertIndexToString[j] = 'a';
	}
	while (1)
	{
		if (indexPass > 0)
		{
			index = indexPass % 26;
			convertIndexToString[pos] = alphabet[index];
			pos--;
			indexPass = indexPass / 26;
		}
		else
		{
			break;
		}
	}
}

void encodeMD5(const char *string, char *md5Encode)
{
	unsigned char digest[16];
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, string, strlen(string));
	MD5_Final(digest, &ctx);
	for (int i = 0; i < 16; i++)
		sprintf(&md5Encode[i * 2], "%02x", (unsigned int)digest[i]);
}

void findPass(int begin, int end, char *md5Encode, int passwordLength, int rank)
{
	char pass[passwordLength];
	char temp_md5Encode[33];
	while (1)
	{
		convertIndexToString(begin, passwordLength, pass);
		encodeMD5(pass, temp_md5Encode);
		if (!strcmp(temp_md5Encode, md5Encode))
		{
			printf("Password found is:\t%s\t.On rank: %d\n", pass, rank);
			break;
		}
		else if (begin == end)
		{
			break;
		}
		else
		{
			begin++;
		}
	}
	return;
}

void processOnRank(char *md5Encode, int numberProcess, int passwordLength, int rank)
{
	char hostname[50];
    clock_t tStart,tEnd;
	int begin, end;
	int totalPassword = pow(26, passwordLength);
	int part = totalPassword / numberProcess;
	begin = part * (rank);
	if (rank != numberProcess - 1)
	{
		end = begin + part - 1;
	}
	else
	{
		end = totalPassword - 1;
	}
    tStart = clock();
	gethostname(hostname, 50);
	printf("My rank : %d , hostname: %s \n", rank, hostname);
	printf("Index begin: %d , Index end: %d\n", begin, end);
	findPass(begin, end, md5Encode, passwordLength, rank);
    tEnd = clock();
    float diff ((float)tEnd-(float)tStart);
	printf("Processing Time: %f second on rank %d \n", 	diff / CLOCKS_PER_SEC, rank);
	exit;
}
