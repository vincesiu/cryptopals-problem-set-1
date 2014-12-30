//Vincent Siu
//2014-12-25
//Repeating Key XOR

#include "fileReader.h"

//filenames
#define input_filename "input1.txt"
#define output_filename "input3.txt"

//Key information, lets go portability!
#define XORKeyString "ahegao"
#define XORKeyLength 6


/**
 * Changes a character which holds a binary hexadecimal into the corresponding binary character
 * @param  input Binary represenation of a hexadecimal number
 * @return       0-9, a-f
 */
char binaryToHex(unsigned char input)
{
	if ( (input >= 0) && (input <= 9) )
		return input + '0';
	if ( (input >= 10) && (input <= 15) )
		return input + 'a' - 10;
	fprintf(stderr, "Error[charToHex]: passed a nonhexadecimal input");
	exit(1); 
}



/**
 * Changes a character string into the hexadecimal representation that it would have
 * @param  inputString         /
 * @param  inputLength         /
 * @param  outputStringAddress /
 * @return                     returns length of the outputString 
 */
int charStringToHex(char *inputString, int inputLength, char **outputStringAddress)
{
	int outputLength = 2 * inputLength;
	char *outputString = (char *) malloc (sizeof (char) * (outputLength + 1));

	if (outputString == NULL)
	{
		fprintf(stderr, "Error[charToBinary]: error when allocating memory");
		exit(1);
	}
	for (int i = 0; i < inputLength; i++)
	{
		char temp = inputString[i];

		outputString[i*2] = binaryToHex( ( temp >> 4 ) & 15);
		outputString[i*2 + 1] = binaryToHex(  (temp & 15) );
	}
	outputString[outputLength] = '\0';
	*outputStringAddress = outputString;
	return outputLength;
}



int main()
{
//Reading input
//////////////////////////////
char *inputString = NULL;
char *inputFile = input_filename;
int inputLength = readTextFromFile(&inputString, input_filename);


//Applying the XOR
////////////////////////////////
char *XORKey = XORKeyString;
for (int i = 0; i < inputLength; i++)
	inputString[i] = inputString[i] ^ XORKey[i % XORKeyLength];

//Changing it to Hex
///////////////////////////
char *outputString = NULL;
charStringToHex(inputString, inputLength, &outputString);


//Writing to output
/////////////////////////////
char *outputFile = output_filename;
writeTextToFile (outputString, outputFile);

//Cleanup
/////////////////////////////
free (outputString);
free (inputString);
return 0;
}