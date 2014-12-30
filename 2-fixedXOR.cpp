// Vincent Siu
// 2014-12-19
// Creating a fixed xor function

#include "fileReader.h"
#include <iostream>

//Change these based on the filenames you want to use
#define input_filename1 "input1.txt"
#define input_filename2 "input2.txt"
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


/**
 * Given a byte which encodes a hex value, changes it into a character value
 * @param  inputHex Given hex encoding
 * @return          Corresponding character for the hex encoding
 */
char hexByteToChar (unsigned char inputHex)
{
	if (inputHex >= 0 && inputHex <= 9)
		return (inputHex + '0');
	else if (inputHex >= 10 && inputHex <= 15)
		return (inputHex + 'a' - 10);
	else
	{
		fprintf (stderr, "Error[hexByteToChar]: Passed an invalid hex byte, cannot convert to character");
		exit(1);
	}
}

/**
 * XORs the binary of two hex values
 * @param  input1 First hex value
 * @param  input2 Second hex value
 * @return        Final XORed hex value
 */
unsigned char XOR_4bits (unsigned char input1, unsigned char input2)
{
	unsigned char output = 0;
	for (int i = 3; i >= 0; i--)
	{
		output = output << 1;
		output += ((input1 >> i) & 1) ^ ((input2 >> i) & 1); 	
	}
	return output;
}




int main()
{
	
	char *inputFile1 = input_filename1;
	char *inputFile2 = input_filename2;

	char *inputString1 = NULL;
	char *inputString2 = NULL;

	int inputLength1 = readTextFromFile (&inputString1, inputFile1);
	int inputLength2 = readTextFromFile (&inputString2, inputFile2);

	if (inputLength1 != inputLength2)
	{
		fprintf (stderr, "Error[fixedXOR]: the inputs did not provide equal length buffers, please edit and try again");
		exit(1);
	}

	unsigned char inputHexByte1;
	unsigned char inputHexByte2;
	unsigned char outputHexByte;
	char *outputString = (char *) malloc (sizeof(char) * (inputLength1 + 1));

	int i;
	for (i = 0; i < inputLength1; i++)
	{
		inputHexByte1 = charToHexBit(inputString1[i]);
		inputHexByte2 = charToHexBit(inputString2[i]);
		outputHexByte = XOR_4bits(inputHexByte1, inputHexByte2);
		outputString[i] = hexByteToChar(outputHexByte);
	}
	outputString[i] = '\0';

	char *outputFile = output_filename;
	writeTextToFile (outputString, outputFile);

	free (inputString1);
	free (inputString2);
	free (outputString);
	return 0;
}