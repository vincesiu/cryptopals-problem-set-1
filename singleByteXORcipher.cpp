//Vincent Siu
//2014-12-20
//Single byte XOR cipher

#include "fileReader.h"

//If you want to change the file names, put them here
#define input_filename "input1.txt"
#define output_filename "output.txt"

/**
 * Given a character value, changes it to the corresponding bit representation and returns the byte in the form in an unsigned char
 * @param  inputChar Given character which is one of the hexadecimal characters, 0-9, a-f
 * @return           Bit representation of the hex character, eg. a becomes 1010 (in binary) or 10 (in decimal)
 */
unsigned char charToHexBit (char inputChar)
{
	if (inputChar >= '0' && inputChar <= '9')
		return (inputChar - '0');
	else if (inputChar >= 'a' && inputChar <='f')
		return (inputChar - 'a' + 10);
	else if (inputChar >= 'A' && inputChar <= 'F')
		return (inputChar - 'A');	
	else
	{
		fprintf (stderr, "Error[CharToHexBit]: Passed a character that was not a hexadecimal character.");
		exit(1);
	} 
}



int main()
{

char *inputString = NULL;
char *inputFile = input_filename;
int inputLength = readTextFromFile(&inputString, inputFile);
}