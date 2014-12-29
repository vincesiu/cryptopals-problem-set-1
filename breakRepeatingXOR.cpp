//Vincent Siu
//2014-12-26
//Breaking Repeating XOR

#include "fileReader.h"
#include <cstring>
#include <map>

#define maxExpectedKeyLength 40

#define input_filename "input1.txt"
#define output_filename "output.txt"

//String of win. This is here for reference
#define char_frequency_list "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRXTUVWXYZ1234567890" //"etaoin shrdlucETAOINSHRDLUC"
#define char_frequency_list_length 63

int calculateHammingDistance(char *inputString1, char *inputString2, int inputLength);
int removeNewlines(char *inputString, int inputLength);
char charToBase64(char input);
int base64toBase256(char *inputString, int inputLength, char **outputStringAddress);
void findTopTenKeyLengths(float *inputString, int inputLength, int **outputStringAddress);
bool decipherRepeatingXOR(int keyLength, char *inputString, int inputLength, char **key);
int check_valid_characters (char *inputString, int inputLength);
void applyXORKeyToString (char *inputString, int inputLength, char key, char **outputStringAddress);



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

int *list_topTenKeyLengths = NULL;
findTopTenKeyLengths(list_normalizedEditDistances, maxExpectedKeyLength + 1, &list_topTenKeyLengths);

for (int i = 0; i < 10; i++)
{
	printf("top %d: %d\n", i, list_topTenKeyLengths[i]);
}

char *key = NULL;
/*
for (int i = 0; i < 10; i++)
{
	if (decipherRepeatingXOR(list_topTenKeyLengths[i], parsed_inputString, inputLength, &key))
	{
		for (int j = 0; j < list_topTenKeyLengths[i]; j++)
			printf("%c", key[j]);
	}
}
*/
for (int i = 0; i < inputLength; i++)
	printf("%.2x ", parsed_inputString[i]);
int keyLength = 0;
for (int i = 2; i < 50; i++)
{
	if (decipherRepeatingXOR(i, parsed_inputString, inputLength, &key))
	{
		for (int j = 0; j < list_topTenKeyLengths[i]; j++)
			printf("%c", key[j]);
		keyLength = i;
		break;
	}
}

char *outputString = (char *) malloc (sizeof (char) * (inputLength + 1));
int idx = 0;
for ( int i = 0; i < inputLength; i++)
{
	outputString[i] = parsed_inputString[i] ^ key[ i%keyLength ];
}
outputString[inputLength] = '\0';
char *outputFile = output_filename;

writeTextToFile (outputString, outputFile);

free (outputString);
free (raw_inputString);
free (parsed_inputString);
free (list_normalizedEditDistances);
free (list_topTenKeyLengths);


}



/**
 * The big one.
 * @param  keyLength   /
 * @param  inputString /
 * @param  inputLength /
 * @param  key         If key is found, this will point to a char string
 * @return             true found the key, false if didn't find the key
 */
bool decipherRepeatingXOR(int keyLength, char *inputString, int inputLength, char **keyAddress)
{
	char **list_ptrsToBlocks = (char **) malloc (sizeof (char*) * keyLength);
	int length = inputLength / keyLength;
	int leftovers = inputLength % keyLength;
	for (int i = 0; i < keyLength; i++)
	{
		int currentLength = length + 1;
		if (i < leftovers)
			currentLength ++;
		list_ptrsToBlocks[i] = (char *) malloc (sizeof (char) * currentLength);
		int idx = 0;
		for (int j = i; j < inputLength; j += keyLength, idx++)
			list_ptrsToBlocks[i][idx] = inputString[j];
		list_ptrsToBlocks[i][idx] = '\0';
	}


	char *frequencyList = char_frequency_list;
	char *key = (char *) malloc (sizeof (char) * (keyLength + 1));
	bool foundKey;
	for (int i = 0; i < keyLength; i++)
	{
		foundKey = false;
		int currentLength = length;
		if (i < leftovers)
			currentLength++;
		char *temp = NULL;

			std::map<char, int> frequencyList_Input; 
        	for (int j = 0; j < currentLength; j++)
	        {
	        	char temp = list_ptrsToBlocks[i][j];
	         	if (frequencyList_Input.count(temp) == 1)
	         		frequencyList_Input[temp]++;
	         	else 
	         		frequencyList_Input[temp] = 1;
	        }    
	         
	        char mostFrequentChar;
	        int maxCount = 0;
			//int der = 0;
	        for (std::map<char, int>::iterator itr = frequencyList_Input.begin(); itr != frequencyList_Input.end(); itr++)
	        {
	        	if ((itr->second) > maxCount)
	         	{
	         		mostFrequentChar = (itr->first);
	         		maxCount = (itr->second);
	         	}
				//printf("%.2x : %d\n", unsigned char (itr->first), itr->second);
				//der++;
	        }
		//	printf("\n");







		for (int j = 0; j < char_frequency_list_length; j++)
		{
			char trialKey = mostFrequentChar ^ frequencyList[j];
			if (trialKey == '&')
			{
				printf("\n");
			}
			applyXORKeyToString(list_ptrsToBlocks[i], currentLength, trialKey, &temp);
			if (check_valid_characters(temp, currentLength) == 1)
			{
				key[i] = trialKey;
				foundKey = true;
				free(temp);
				break;
			}

			free(temp);
			temp = NULL;
		}
		if (foundKey == false)
			break;
	}
	


	for (int i = 0; i < keyLength; i++)
		free(list_ptrsToBlocks[i]);
	free(list_ptrsToBlocks);
	if (foundKey == false)
	{
		free (key);
		return false;
	}
	else
	{
		*keyAddress = key;
		return true;
	}
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
	return outputLength;
}

/**
* Some hacked together code...DEFINITELY not portable haha
* @param inputString         [description]
* @param inputLength         [description]
* @param outputStringAddress [description]
*/
void findTopTenKeyLengths(float *inputString, int inputLength, int **outputStringAddress)
{
	int *outputString = (int *)malloc(sizeof (int)* 10);
	float *temp = (float *)malloc(sizeof (float)* inputLength);
	memcpy(temp, inputString, sizeof(float)* inputLength);
	for (int i = 0; i < 10; i++)
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


/**
 * Checks if the string has all valid characters for a text string.
 * I consider the following ranges of the ASCII table to be invalid:
 * 0-8, 11-12, 14-31, 127 inclusive
 * Also, considering how ASCII only encodes the first 127 characters, any characters from 128 to 255 are invalid
 * Some additional restrictions:
 * 1. if a string doesn't have a space in it, then it is considered invalid.
 * 2. if the string has a backslash in it, then it is considered invalid
 * 3. if it doesn't have matching open and closing brackets, then it is considered invalid
 * 4. ^same with parentheses
 * 5. "<" and ">" are considered invalid
 * @param  inputString Ptr to given CString 
 * @param  inputLength Length of given CString
 * @return             1 If the string is valid text, and -1 if the string is invalid text
 */
int check_valid_characters (char *inputString, int inputLength)
{
    for (int i = 0; i < inputLength; i++)
    {
		unsigned char tester = (unsigned char) inputString[i];
        if (tester >= 0  &&  tester <= 8)
            return -1;
        if (tester >= 11 &&  tester <= 12)
            return -1;
        if (tester >= 14 &&  tester <= 31)
            return -1;
        if (tester >= 127)
            return -1;

		//makeshift validation
		if (tester == 37) //%
			return -1;
		if (tester == 126) //~
			return -1;
		if (tester == 35) //#
			return -1;
    }
	/*
	int openBrackets = 0;
	int openParentheses = 0;
	bool foundSpace = false;
    for (int i = 0; i < inputLength; i++)
    {
		unsigned char tester = (unsigned char) inputString[i];
        if (tester >= 0  &&  tester <= 8)
            return -1;
        if (tester >= 11 &&  tester <= 12)
            return -1;
        if (tester >= 14 &&  tester <= 31)
            return -1;
        if (tester >= 127)
            return -1;
		if (tester == 32)
			foundSpace = true;
		if (tester == 40)
			openParentheses++;
		if (tester == 41)
			openParentheses--;
		if (tester == 123)
			openBrackets++;
		if (tester == 125)
			openBrackets--;
		if ((openBrackets < 0) || (openParentheses < 0))
			return -1;
		if (tester == 92)
			return -1;
		if (tester == 60 || tester == 62)
            return -1; 
    }
	if ((openBrackets > 0) || (openParentheses > 0))
		return -1;
	if (!foundSpace)
		return -1;
		*/
    return 1;
}




/**
 * Given an input string and a byte-sized key, this function will xor all the characters in the input string 
 * and place the output in the outputString
 * Example usages:       applyXORKeyToString (inputString, inputLength, key, &outputString);
 * @param inputString         Ptr to input CSTring to be XORed
 * @param inputLength         Length of the CString above
 * @param key                 Byte key that will be used to encode
 * @param outputStringAddress Address to ptr of outputString
 */
void applyXORKeyToString (char *inputString, int inputLength, char key, char **outputStringAddress)
{
    char *outputString = *outputStringAddress;
    if (outputString != NULL)
    {
        fprintf(stderr, "Error[applyXORKeyToString]: Incorrect usage of arguments for this function");
        exit(1);
    }
    outputString = (char *) malloc (sizeof (char) * (inputLength + 1));
    for (int i = 0; i < inputLength; i++)
        outputString[i] = inputString[i] ^ key;
    outputString[inputLength] = '\0';
    *outputStringAddress = outputString;
}
