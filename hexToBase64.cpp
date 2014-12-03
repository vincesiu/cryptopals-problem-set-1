//Vincent Siu
//2014-12-02
//Hexadecimal to Base64
//Takes in a string of hexadecimal from standard input
//Prints the string converted to base64 to standard output

#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>

using namespace std;

int interpretBits( unsigned char base64Bits ) 
{
	if (base64Bits >= 0 && base64Bits <= 25 )
		return base64Bits + 'A';
	else if (base64Bits >= 26 && base64Bits <= 51)
		return base64Bits - 26 + 'a';
	else if (base64Bits >= 52 && base64Bits <= 61 )
		return base64Bits - 52 + '0';
	else if (base64Bits == 62)
		return '+';
	else if (base64Bits == 63)
		return '/';
	cout << "Error: Program should never reach this point";
	exit(1);

}
int main()
{
	ifstream myFile;
	myFile.open("file.txt");

if (sizeof(int) != 4)
{
	cout << "Error: this program won't work if the size of an int is not equal to 4 bytes.";
	exit(1);
}

//Taking in string of hexadecimals
string input;
myFile >> input;
int inputLength = input.length();

int startIndex = 3 - (inputLength % 3);
if (startIndex == 3)
	startIndex = 0;

int *intArray = (int *) malloc (sizeof(int) * (inputLength + startIndex) );

for ( int i = 0; i < startIndex; i++)
	intArray[i] = 0;

for ( int i = startIndex; i < inputLength; i++ )
{ 
if (input[i] >= '0' && input[i] <= '9' )
	intArray[i] = input[i] - '0';
else if (input[i] >= 'a' && input[i] <= 'f' )
	intArray[i] =  input[i] - 'a' + 10;
else
	{
	cout << "Error: invalid hexadecimal character";
	exit(1);
	}
}

//Now I have an array of ints in contiguous order, just what I need to translate into Base 64

int outputLength = inputLength / 3;         //This will be the length of the array of base64 numbers
double temp = (double) inputLength / 3;
if (temp > outputLength)
	outputLength++;
outputLength *= 2;

unsigned char *base64Array = (unsigned char *) malloc ( sizeof(unsigned char) * outputLength);
unsigned char holder;
int i, j;
for ( i = 0, j = 0; i < inputLength; i += 3, j += 2)
{

holder = ( intArray[i] << 2 ) + (( intArray[i + 1] & 12 ) >> 2 ) ;
base64Array[j] = interpretBits(holder);
holder = ( (intArray[i + 1] & 3 ) << 4 ) + intArray[i + 2];
base64Array[j + 1] = interpretBits(holder);
}

for (int i = 0; i < outputLength; i++)
	cout << base64Array[i];
return 0;
}



