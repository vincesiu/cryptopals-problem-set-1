//Vincent Siu
//2014-12-22
//detectSingleCharXOR.cpp
//Takes in multiple lines of text, and only writes to output the valid lines

#include "fileReader.h"
#include <map>

//If you want to change the file names, put them here
#define input_filename "input2.txt"
#define output_filename "output.txt"

//String of win. This is here for reference
#define char_frequency_list "etaoin shrdlucETAOINSHRDLUC"


void applyXORKeyToString(char *inputString, int inputLength, char key, char **outputStringAddress);
unsigned char charToHexBit (char inputChar);
int check_valid_characters(char *inputString, int inputLength);
int changeHexCharsToChars(char *inputString, int inputLength, char **outputString);

int main()
{

	// Initializations, parsing the multi line file
	//////////////////////////////////////////////////////////////////
    char *inputString = NULL;
    char *inputFile = input_filename;
    int inputLength = readTextFromFile (&inputString, inputFile);    



    char **ptrs_lineInput = (char **) malloc (sizeof (char *) );
    int *length_lineInput = (int *) malloc (sizeof (int));
    if (ptrs_lineInput == NULL || length_lineInput == NULL)
    {
    	fprintf( stderr, "Error[singleByteXORcipher]: error allocating memory");
    	exit(1);
    }
	ptrs_lineInput[0] = NULL;
    length_lineInput[0] = 0;

	int i;
    int idx_currentLine = 0;
    for (i = 0; i < inputLength; i++)
    {
    	if (inputString[i] != '\n')
    		length_lineInput[idx_currentLine]++;
    	else
    	{
    		int lengthCurrentLine = length_lineInput[idx_currentLine];
			length_lineInput[idx_currentLine] = changeHexCharsToChars ( (inputString + i - lengthCurrentLine), lengthCurrentLine, (ptrs_lineInput + idx_currentLine) );
			idx_currentLine++;
			length_lineInput = (int *) realloc (length_lineInput, sizeof (int) * (idx_currentLine + 1));
            ptrs_lineInput = (char **) realloc (ptrs_lineInput, sizeof (char *) * (idx_currentLine + 1));
            length_lineInput[idx_currentLine] = 0;
            ptrs_lineInput[idx_currentLine] = NULL;
    	}
		
    }
	if (inputString[i-1] != '\n')
    {
    	int lengthCurrentLine = length_lineInput[idx_currentLine];
    	length_lineInput[idx_currentLine] = changeHexCharsToChars ( (inputString + i - lengthCurrentLine), lengthCurrentLine, (ptrs_lineInput + idx_currentLine) );
    	idx_currentLine++;
    }



	for ( int l = 0; l < idx_currentLine; l++)
	{
		printf("%d:  ", length_lineInput[l]);
		for ( int o = 0; o < length_lineInput[l]; o++)
		{
			printf("%.2x", (unsigned char) ptrs_lineInput[l][o]);
		}
		printf("\n");
	}




    int numberOfLines = idx_currentLine;
	int idx_outputString = 0;
	int outputSize = 61;
	char *outputString = (char *) malloc (sizeof (char) * outputSize);
    for (i = 0; i < numberOfLines; i++)
    {
         char *raw_charString = ptrs_lineInput[i];
         int charStringLength = length_lineInput[i];
         //Calculating character frequencies of the input string and getting most frequent character
         ////////////////////////////////////////////////////////////////////////////////////////////
         std::map<char, int> frequencyList_Input; 
         for (int j = 0; j < charStringLength; j++)
         {
         	char temp = raw_charString[j];
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
         for (int j = 0; j < 27; j++)
         {
         	testKey = mostFrequentChar ^ mostFrequentCharList[j];
         	applyXORKeyToString(raw_charString, charStringLength, testKey, &deciphered_charString);
         	if ( check_valid_characters(deciphered_charString, charStringLength) == 1)
         	{
         		char *outputFile = output_filename;
         		//foundValidKey = true;
				//break;









				int j = 0;
				int k = 0;
				for (k = idx_outputString; k < (idx_outputString + 60); k++, j++)
				{
					outputString[k] = deciphered_charString[j];
				}
				outputString[k] = '\0';
				outputSize += 60;
				idx_outputString += 60;
				outputString = (char *)realloc(outputString, sizeof (char)* outputSize);
         	}
        		free (deciphered_charString);
        		deciphered_charString = NULL;
         }    
		 /*
		 if (foundValidKey)
		 {
			 int j = 0;
			 int k = 0;
			 for (k = idx_outputString; k < (idx_outputString + 60); k++, j++)
			 {
				 outputString[k] = deciphered_charString[j];
			 }
			 outputString[k] = '\0';
			 outputSize += 60;
			 idx_outputString += 60;
			 outputString = (char *) realloc (outputString, sizeof (char) * outputSize);
			 free (deciphered_charString);
		 }
		 */
	}

	char *outputFile = output_filename;
	writeTextToFile(outputString, outputFile);
	free (outputString);
    free (inputString);
    free (length_lineInput);
	//free (modified_inputString);
    for (int j = 0; j < numberOfLines; j++)
    {
    	free (ptrs_lineInput[j]);
    }
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
        return (inputChar - 'A' + 10);   
    else
    {
        fprintf (stderr, "Error[CharToHexBit]: Passed a character that was not a hexadecimal character.");
        exit(1);
    }
}


/**
 * Checks if the string has all valid characters for a text string.
 * I consider the following ranges of the ASCII table to be invalid:
 * 0-8, 11-12, 14-31, 127 inclusive
 * Also, considering how ASCII only encodes the first 127 characters, any characters from 128 to 255 are invalid
 * @param  inputString Ptr to given CString 
 * @param  inputLength Length of given CString
 * @return             1 If the string is valid text, and -1 if the string is invalid text
 */
int check_valid_characters (char *inputString, int inputLength)
{
    for (int i = 0; i < inputLength; i++)
    {
        if ((unsigned char) inputString[i] >= 0  &&  (unsigned char) inputString[i] <= 8)
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




/**
 * Given a string of characters which is the hexadecimal representation of a string of ASCII characters,
 * converts the string into the given characters
 * usage:      changeHexStringToChars (inputString, inputLength, &outputString);
 * @param inputString         Ptr to given input string
 * @param inputLength         Length of input string
 * @param outputStringAddress Address of ptr to output string
 */
int changeHexCharsToChars (char *inputString, int inputLength, char **outputStringAddress)
{
	char *outputString = *outputStringAddress;

	if (outputString != NULL || (inputLength % 2 != 0) )
	{
		fprintf(stderr, "Error[changeHexBinaryToChars]: Incorrect usage of arguments for this function");
		exit(1);
	}

	unsigned char temp1;
    unsigned char temp2;
    int i = 0;
    int j = 0;
    int outputLength = inputLength / 2;
    outputString = (char *) malloc (sizeof (int) * (outputLength + 1));
    if (outputString == NULL)
    {
    	fprintf(stderr, "Error[changeHexBinaryToChars]: Error allocating memory");
    	exit(1);
    }

    for ( ; i < inputLength; i+=2, j++ )
    {
		unsigned char temp3;
    	temp1 = charToHexBit(inputString[i]);
    	temp2 = charToHexBit(inputString[i + 1]);
		outputString[j] = ((temp1 << 4) + temp2);
    }

    outputString[j] = '\0';
    
    *outputStringAddress = outputString;
    return outputLength;
}
