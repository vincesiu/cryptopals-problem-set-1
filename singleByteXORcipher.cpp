//Vincent Siu
//2014-12-20
//Single byte XOR cipher, assuming that the plaintext is encoded in ascii
//Not completely robust, known vulnerabilities:
//1. Program can be thrown off if the text string is all capitals since my frequency
//   list only consists of lower case characters
//2. Program can be thrown off if the most frequent character is upper-case. I'll fix this later

#include "fileReader.h"
#include <map>

//If you want to change the file names, put them here
#define input_filename "input1.txt"
#define output_filename "output.txt"

//String of win. this is just up here for reference.

#define lower_char_frequency_list "etaoin shrdlu"



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
 * Checks if the string has all valid characters for a text string.
 * I consider the following ranges of the ASCII table to be invalid:
 * 1-8, 11-12, 14-31, 127 inclusive
 * Also, considering how ASCII only encodes the first 127 characters, any characters from 128 to 255 are invalid
 * @param  inputString Ptr to given CString 
 * @param  inputLength Length of given CString
 * @return             1 If the string is valid text, and -1 if the string is invalid text
 */
int check_valid_characters(char *inputString, int inputLength)
{
	for (int i = 0; i < inputLength; i++)
	{
		if ((unsigned char) inputString[i] >= 1 &&  (unsigned char) inputString[i] <= 8)
			return -1;
		if ((unsigned char) inputString[i] >= 11 &&  (unsigned char) inputString[i] <= 12)
			return -1;
		if ((unsigned char) inputString[i] >= 14 &&  (unsigned char) inputString[i] <= 31)
			return -1;
		if ((unsigned char) inputString[i] >= 127)
			return -1;
	}
	return 1;
}


void applyXORKeyToString(char *inputString, int inputLength, char key, char **outputStringAddress)
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


int main()
{
char *inputString = NULL;
char *inputFile = input_filename;
int inputLength = readTextFromFile(&inputString, inputFile);

if (inputLength % 2 != 0)
{
	fprintf (stderr, "Error[singleByteXORcipher]: passed input data which was not aligned to a byte.");
	free (inputString);
	exit(1);
}


int charStringLength = (inputLength / 2);
char *raw_charString = (char *) malloc (sizeof (char) * (charStringLength + 1) );

unsigned char temp1;
unsigned char temp2;
int i = 0;
int j = 0;
for ( ; i < inputLength; i+=2, j++ )
{
	temp1 = charToHexBit(inputString[i]);
	temp2 = charToHexBit(inputString[i + 1]);
	raw_charString[j] = ( temp1 << 4 ) + temp2;
}
raw_charString[j] = '\0';

std::map<char, int> frequencyList; 
//Inserting the frequencies into my map
for (i = 0; i < charStringLength; i++)
{
	char temp = raw_charString[i];
	if (frequencyList.count(temp) == 1)
		frequencyList[temp]++;
	else 
		frequencyList[temp] = 1;
}


char most_frequent_char;
int max_count = 0;
for (std::map<char, int>::iterator itr = frequencyList.begin(); itr != frequencyList.end(); itr++)
{
	if ((itr->second) > max_count)
	{
		most_frequent_char = (itr->first);
		max_count = (itr->second);
	}
	//printf("%x: %d\n", itr->first, itr->second);
}

int flag = 0;
char *frequency_list = lower_char_frequency_list;
char key;
char *tempOutput = NULL;
for (int i = 0; i < 13; i++)
{
	key = most_frequent_char ^ frequency_list[i];
	applyXORKeyToString(raw_charString, charStringLength, key, &tempOutput);
	if ( check_valid_characters(tempOutput, charStringLength) == 1)
	{
		flag = 1;
		break;
	}
	else
	{
		free (tempOutput);
		tempOutput = NULL;
	}
}

if (flag == 1)
{
	for (int i = 0; i < charStringLength; i++)
		printf("%c", tempOutput[i]);
}

free (inputString);
free (raw_charString);
return 0;

}