//Vincent Siu
//2014-12-20
//Single byte XOR cipher, assuming that the plaintext is encoded in ascii
//Not completely robust, known vulnerabilities:
//1. [FIXED] Program can be thrown off if the text string is all capitals since my frequency
//   list only consists of lower case characters
//2. [FIXED] Program can be thrown off if the most frequent character is upper-case. I'll fix this later
//3. Program does not work on input strings that are too small where the most frequent character
//   is not one of the top 12.

#include "fileReader.h"
#include <map>

//If you want to change the file names, put them here
#define input_filename "input1.txt"
#define output_filename "output.txt"

//String of win. This is here for reference
#define char_frequency_list "etaoin shrdluETAOINSHRDLU"


void applyXORKeyToString(char *inputString, int inputLength, char key, char **outputStringAddress);
unsigned char charToHexBit (char inputChar);
int check_valid_characters(char *inputString, int inputLength);

int main()
{

	// Initializations
	//////////////////////////////////////////////////////////////////
    char *inputString = NULL;
    char *inputFile = input_filename;
    int inputLength = readTextFromFile(&inputString, inputFile);    
    if (inputLength % 2 != 0)
    {
    	fprintf (stderr, "Error[singleByteXORcipher]: passed input data which was not aligned to a byte.");
    	exit(1);
    }    
    int charStringLength = (inputLength / 2);
    char *raw_charString = (char *) malloc (sizeof (char) * (charStringLength + 1) );   
	if (raw_charString == NULL)
	{
		fprintf( stderr, "Error[singleByteXORcipher]: error allocating memory");
		exit(1);
	} 
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


    
    
    //Calculating character frequencies of the input string and getting most frequent character
    ////////////////////////////////////////////////////////////////////////////////////////////
    std::map<char, int> frequencyList_Input; 
    for (i = 0; i < charStringLength; i++)
    {
    	char temp = raw_charString[i];
    	if (frequencyList_Input.count(temp) == 1)
    		frequencyList_Input[temp]++;
    	else 
    		frequencyList_Input[temp] = 1;
    }    
    
    char mostFrequentChar;
    int maxCount = 0;
    for (std::map<char, int>::iterator itr = frequencyList_Input.begin(); itr != frequencyList_Input.end(); itr++)
    {
    	if ((itr->second) > maxCount)
    	{
    		mostFrequentChar = (itr->first);
    		maxCount = (itr->second);
    	}
    }   



    //Using the frequency analysis, testing out all the possible max_frequency characters and creating keys
    //based on that information
    bool foundValidKey = false;
    char *mostFrequentCharList = char_frequency_list;
    char testKey;
    char *deciphered_charString = NULL;
    for (int i = 0; i < 25; i++)
    {
    	testKey = mostFrequentChar ^ mostFrequentCharList[i];
    	applyXORKeyToString(raw_charString, charStringLength, testKey, &deciphered_charString);
    	if ( check_valid_characters(deciphered_charString, charStringLength) == 1)
    	{
    		char *outputFile = output_filename;
    		writeTextToFile(deciphered_charString, output_filename);
    		foundValidKey = true;
			break;
    	}
   		free (deciphered_charString);
   		deciphered_charString = NULL;
    }    


    if (deciphered_charString != NULL)
        free (deciphered_charString);
    free (inputString);
    free (raw_charString);
    return 0;
}













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
        if ((unsigned char) inputString[i] >= 1  &&  (unsigned char) inputString[i] <= 8)
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




/**
 * Given an input string and a byte-sized key, this function will xor all the characters in the input string 
 * and place the output in the outputString
 * Example usages:       applyXORKeyToString (inputString, inputLength, key, &outputString);
 * @param inputString         Ptr to input CSTring to be XORed
 * @param inputLength         Length of the CString above
 * @param key                 Byte key that will be used to encode
 * @param outputStringAddress Address to ptr of outputString
 */
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

