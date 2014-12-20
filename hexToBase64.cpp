//Vincent Siu
//2014-12-02
//Hexadecimal to Base64
//Takes in a string of hexadecimal from standard input
//Prints the string converted to base64 to standard output

#include "fileReader.h"

#define input_filename "file.txt"
#define output_filename "output.txt"

/**
 * Takes a parameter that is in base 64 representation, and returns the appropriate character 
 * @param  base64Bits the least 6 significant bits of this variable encodes data for the base 64 number
 */
char interpretBase64Byte( unsigned char base64Bits ) 
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
	fprintf(stderr, "Error[interpretBase64Byte]: passed a variable which does not encode a base 64 number");
	exit(1);
}





int main()
{
	char *input = NULL;
	char *inputFile = input_filename;
	int inputLength = readTextFromFile(&input, inputFile);

	//Correcting for the start index
	int startIndex = 3 - (inputLength % 3);
	if (startIndex == 3)
		startIndex = 0;


	int *intArray = (int *) calloc (sizeof(int) * (inputLength + startIndex), 1 );
	if (intArray == NULL)
	{
		fprintf(stderr, "Error[hexToBase64]: error allocating memory");
		exit(1);
	}


	for ( int i = startIndex; i < inputLength; i++ )
	{ 
		if (input[i] >= '0' && input[i] <= '9' )
			intArray[i] = input[i] - '0';
		else if (input[i] >= 'a' && input[i] <= 'f' )
			intArray[i] =  input[i] - 'a' + 10;
		else
		{
			fprintf(stderr, "Error[hexToBase64]: invalid hexadecimal character");
			exit(1);
		}
	}

	//Now I have an array of ints in contiguous order, just what I need to translate into Base 64
	int outputLength = inputLength / 3;         //This will be the length of the array of base64 numbers
	double temp = (double) inputLength / 3;
	if (temp > outputLength)
		outputLength++;
	outputLength *= 2;

	char *base64Array = (char *) malloc ( sizeof(char) * outputLength);
	if (intArray == NULL)
	{
		fprintf(stderr, "Error[hexToBase64]: error allocating memory");
		exit(1);
	}
	unsigned char holder;
	int i, j;
	for ( i = 0, j = 0; i < inputLength; i += 3, j += 2)
	{
		holder = ( intArray[i] << 2 ) + (( intArray[i + 1] & 12 ) >> 2 ) ;
		base64Array[j] = interpretBase64Byte(holder);
		holder = ( (intArray[i + 1] & 3 ) << 4 ) + intArray[i + 2];
		base64Array[j + 1] = interpretBase64Byte(holder);
	}
	base64Array[j-1] = '\0';

	char *outputFile = output_filename;
	writeTextToFile(base64Array, outputFile);

	free (input);
	free (base64Array);
	return 0;
}

