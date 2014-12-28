//Vincent Siu
//2014-12-26
//Breaking Repeating XOR

#include "fileReader.h"
#include <cstring>

#define maxExpectedKeyLength 40

#define input_filename "input1.txt"
#define output_filename "output.txt"

int calculateHammingDistance(char *inputString1, char *inputString2, int inputLength);
int removeNewlines(char *inputString, int inputLength);
char charToBase64(char input);
int base64toBase256(char *inputString, int inputLength, char **outputStringAddress);
void findTopFiveKeyLengths(float *inputString, int inputLength, int **outputStringAddress);
bool decipherRepeatingXOR(int keyLength, char *inputString, int inputLength, char **key);


int main()
{
	/*
	char *derp1 = "this is a test";
	char *derp2 = "wokka wokka!!!";
	int number = calculateHammingDistance(derp1, derp2, 14);
	printf( "%d", number);
	*/
char *inputFile = input_filename;
char *raw_inputString = NULL;
int inputLength = readTextFromFile(&raw_inputString, inputFile);
inputLength = removeNewlines (raw_inputString, inputLength);

char *parsed_inputString = NULL;

inputLength = base64toBase256(raw_inputString, inputLength, &parsed_inputString);

/*
for (int i = 0; i < 128; i++)
{
	printf( "%.2x", parsed_inputString[i]);
}
*/
float *list_normalizedEditDistances = (float *) calloc ((maxExpectedKeyLength + 1), sizeof(float));
list_normalizedEditDistances[0] = 1000;
list_normalizedEditDistances[1] = 1000;
for ( int i = 2; i <= maxExpectedKeyLength; i++)
{
	list_normalizedEditDistances[i] = calculateHammingDistance(parsed_inputString, parsed_inputString + i, i) / (float) i;
}

for (int i = 0; i <= maxExpectedKeyLength; i++)
{
	printf("keyLength %d: %f\n", i, list_normalizedEditDistances[i]);
}

int *list_topFiveKeyLengths = NULL;
findTopFiveKeyLengths(list_normalizedEditDistances, maxExpectedKeyLength + 1, &list_topFiveKeyLengths);

for (int i = 0; i < 5; i++)
{
	printf("top %d: %d\n", i, list_topFiveKeyLengths[i]);
}

char *key = NULL;
for (int i = 0; i < 5; i++)
{
	if (decipherRepeatingXOR(list_topFiveKeyLengths[i], parsed_inputString, inputLength, &key))
	{
		for (int j = 0; j < list_topFiveKeyLengths[i]; j++)
			printf("%c", key[j]);
	}
}


free (raw_inputString);
free (parsed_inputString);
free (list_normalizedEditDistances);
free (list_topFiveKeyLengths);


}


bool decipherRepeatingXOR(int keyLength, char *inputString, int inputLength, char **key)
{
	char **list_ptrsToBlocks = (char **) malloc (sizeof (char*) * keyLength);
	int length = inputLength / keyLength;
	int leftovers = inputLength % keyLength;
	for (int i = 0; i < keyLength; i++)
		list_ptrsToBlocks[i] = (char *) malloc (sizeof (char) * length);
	for (int i = 0; i < leftovers; i++)
		list_ptrsToBlocks[i] = (char *) realloc ( list_ptrsToBlocks[i], sizeof (char) * (length + 1) );
	//check if the ^ code is right in its lengths


	//now copy over the correct contents into each block
	//Then, apply the single XOR code to this, remember to slacken the restrictions on viable strings
	
	//lehgo

	for (int i = 0; i < keyLength; i++)
		free(list_ptrsToBlocks[i]);
	free(list_ptrsToBlocks);
	return false;
}

/**
* Calculate the hamming distance
* @param  inputString1 /
* @param  inputString2 /
* @param  inputLength  /
* @return              Returns hamming distance
*/
int calculateHammingDistance(char *inputString1, char *inputString2, int inputLength)
{
	int hammingDistance = 0;
	for (int i = 0; i < inputLength; i++)
	{
		char temp = inputString1[i] ^ inputString2[i];
		for (int j = 0; j < 8; j++)
		{
			hammingDistance = hammingDistance + (temp & 1);
			temp = temp >> 1;
		}
	}
	return hammingDistance;
}

/**
 * Intuitive
 * @param  inputString /
 * @param  inputLength /
 * @return             Returns length of the adjusted string
 */
int removeNewlines(char *inputString, int inputLength)
{
	int idx = 0;
	for (int i = 0; i < inputLength; i++)
	{
		if (inputString[i] != '\n')
		{
			inputString[idx] = inputString[i];
			idx++;
		}
	}
	inputString[idx] = '\0';
	return idx;
}

/**
* Takes a character which represents a base 64 number, and encodes into into binary
* @param  input /
* @return       binary encoding
*/
char charToBase64(char input)
{

	if (input >= 'A' && input <= 'Z')
		return (input - 'A');
	else if (input >= 'a' && input <= 'z')
		return (input - 'a' + 26);
	else if (input >= '0' && input <= '9')
		return (input - '0' + 52);
	else if (input == '+')
		return 62;
	else if (input == '/')
		return 63;
	else if (input == '=') //Cool! Learned that this is just extra padding on the end of base64 encodings so the total length 
		return 0;          //ends up being a multiple of 4
	fprintf(stderr, "Error[charToBase64]: passed a characters which is not an encoding of a base 64 number");
	exit(1);
}

/**
* Changes a string which has the characters which represent a base 64 encoding, and then puts it into the binary
* @param  inputString         [description]
* @param  inputLength         [description]
* @param  outputStringAddress [description]
* @return                     [description]
*/
int base64toBase256(char *inputString, int inputLength, char **outputStringAddress)
{
	if (inputLength % 4 != 0)
	{
		fprintf(stderr, "Error[base64toBase256]: Passed an invalid input string, does not translate correctly into an output)");
		exit(1);
	}
	int outputLength = (inputLength / 4) * 3;
	char *outputString = (char *)malloc(sizeof (char)* (outputLength + 1));
	int idx_output = 0;
	for (int idx_input = 0; idx_input < inputLength; idx_input += 4, idx_output += 3)
	{
		outputString[idx_output] = (charToBase64(inputString[idx_input]) << 2) + (charToBase64(inputString[idx_input + 1]) >> 4);
		outputString[idx_output + 1] = (charToBase64(inputString[idx_input + 1]) << 4) + (charToBase64(inputString[idx_input + 2]) >> 2);
		outputString[idx_output + 2] = (charToBase64(inputString[idx_input + 2]) << 6) + charToBase64(inputString[idx_input + 3]);
	}
	outputString[outputLength] = '\0';
	*outputStringAddress = outputString;
	return inputLength;
}

/**
* Some hacked together code...DEFINITELY not portable haha
* @param inputString         [description]
* @param inputLength         [description]
* @param outputStringAddress [description]
*/
void findTopFiveKeyLengths(float *inputString, int inputLength, int **outputStringAddress)
{
	int *outputString = (int *)malloc(sizeof (int)* 5);
	float *temp = (float *)malloc(sizeof (float)* inputLength);
	memcpy(temp, inputString, sizeof(float)* inputLength);
	for (int i = 0; i < 5; i++)
	{
		float min = temp[0];
		int index = 0;
		for (int j = 0; j < inputLength; j++)
		{
			if (temp[j] < min)
			{
				min = temp[j];
				index = j;
			}
		}
		outputString[i] = index;
		temp[index] = 10000;
	}
	*outputStringAddress = outputString;
	free(temp);
}







