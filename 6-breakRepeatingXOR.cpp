//Vincent Siu
//2014-12-26
//Breaking Repeating XOR
//Very proud of myself! Abusing functions so that my main function code is relatively short! I love it
//Nothing was sweeter than the sight of seeing the Vanilla Ice lyrics come up on the output screen...

//Bugs:
//1. [Fixed] Need to do something about the padding at the end, and how it invalidates the string
//2. [Fixed] Test if this works using the minimal frequency list
//3. Make everything prettier
//4. [Fixed] Do the normalized edit distance correctly, so the target keylength shows up at the very least in the top ten

#include "fileReader.h"
#include <cstring>
#include <map>

//Filenames
#define input_filename "input3.txt"
#define output_filename "output.txt"

//String of win. This is here for reference
#define char_frequency_list "etaoin shrdluETAOINSHRDLU" //"abcdefghjiklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ"//
#define char_frequency_list_length 27 //53 

//Expected Keys
#define maxExpectedKeyLength 40

//Debugging?
#define debugging_mode true

//Function Prototypes
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
	//Parsing the input
	char *inputFile = input_filename;
	char *raw_inputString = NULL;
	int inputLength = readTextFromFile(&raw_inputString, inputFile);	

	inputLength = removeNewlines (raw_inputString, inputLength);	

	char *parsed_inputString = NULL;
	inputLength = base64toBase256(raw_inputString, inputLength, &parsed_inputString);	
	

	if (debugging_mode)
	{
		printf("Hex values of input");
		for (int i = 0; i < inputLength; i++)
			printf("%.2x ", parsed_inputString[i]);
		printf("\n");
	}	

	//Creating a list of normalized edit distances for different keylength blocks
	//This will help in determining the most likely candidates for size of the keylength
	//Assigning each index of the list as a possible keylength (and ignoring a keylength of 0 and 1)
	float *list_distances = (float *) malloc (sizeof(float) * (maxExpectedKeyLength + 1));   
	list_distances[0] = 1000;
	list_distances[1] = 1000;	

	//Calculating the normalized hamming distance, using an average taken across four blocks
	for ( int i = 2; i <= maxExpectedKeyLength; i++)
	{
	    float temp1 = calculateHammingDistance(parsed_inputString, parsed_inputString + i, i) / (float) i;
		float temp2 = calculateHammingDistance(parsed_inputString, parsed_inputString + 2 * i, i) / (float) i;
		float temp3 = calculateHammingDistance(parsed_inputString, parsed_inputString + 3 * i, i) / (float) i;
		float temp4 = calculateHammingDistance(parsed_inputString, parsed_inputString + 4 * i, i) / (float) i;
		list_distances[i] = (temp1 + temp2 + temp3 + temp4) / 4;
	}	
	

	if (debugging_mode)
	{
		for (int i = 0; i <= maxExpectedKeyLength; i++)
		{
			printf("keyLength %d: %f\n", i, list_distances[i]);
		}
	}	
	

	int *list_topTenKeyLengths = NULL;
	findTopTenKeyLengths(list_distances, maxExpectedKeyLength + 1, &list_topTenKeyLengths);	
	

	if (debugging_mode)
	{
		for (int i = 0; i < 10; i++)
		{
			printf("top %d: %d\n", i, list_topTenKeyLengths[i]);
		}
	}	
	
	

	char *key = NULL;
	int keyLength = 0;	

	//This is where the heavy computation is done. We're performing brute force attacks, using the top
	//ten candidates of keylength
	for (int i = 0; i < 10; i++)
	{
		if (decipherRepeatingXOR(list_topTenKeyLengths[i], parsed_inputString, inputLength, &key))
		{
			keyLength = list_topTenKeyLengths[i];
			break;
		}
	}	

	if (debugging_mode)
	{
		for (int i = 0; i < keyLength; i++)
			printf("%c", key[i]);
	}	

	char *outputString = NULL;
	//If the key is found, then the keyLength will be nonzero
	if (keyLength != 0)
	{
		outputString = (char *) malloc (sizeof (char) * (inputLength + 1));
		for ( int i = 0; i < inputLength; i++)
			outputString[i] = parsed_inputString[i] ^ key[ i%keyLength ];
		outputString[inputLength] = '\0';	

		char *outputFile = output_filename;
		writeTextToFile (outputString, outputFile);
	}	
	

	//Cleaning it all up
	free (raw_inputString);
	free (parsed_inputString);
	free (list_distances);
	free (list_topTenKeyLengths);
	free (key);
	free (outputString);	

	if (keyLength == 0)
		return 1;
	else
		return 0;
}





/**
 * The big one. Tries to break an inputstring, given a certain keylength
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
	//So, since our goal is to do some fancy work with blocks
	//We might have an input that can not be split evenly into 
	//all the blocks, so some blocks may be larger than others

	//Creating all the blocks, and writing the correct data into each one
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

		//Performing frequency analysis
		//////////////////////////////////////////
		char *temp = NULL;
        char mostFrequentChar;
        int maxCount = 0;
		std::map<char, int> frequencyList_Input; 

       	for (int j = 0; j < currentLength; j++)
        {
        	char temp = list_ptrsToBlocks[i][j];
         	if (frequencyList_Input.count(temp) == 1)
         		frequencyList_Input[temp]++;
         	else 
         		frequencyList_Input[temp] = 1;
        }    

        for (std::map<char, int>::iterator itr = frequencyList_Input.begin(); itr != frequencyList_Input.end(); itr++)
        {
        	if ((itr->second) > maxCount)
         	{
         		mostFrequentChar = (itr->first);
         		maxCount = (itr->second);
         	}
        }



        //Running through all the possibilities for the most frequent character
        //Trying all the different trial keys
        //////////////////////////////////////////////////////
		for (int j = 0; j < char_frequency_list_length; j++)
		{
			char trialKey = mostFrequentChar ^ frequencyList[j];
			applyXORKeyToString(list_ptrsToBlocks[i], currentLength, trialKey, &temp);
			if (check_valid_characters(temp, currentLength) == 1)
			{

				if (debugging_mode)
					printf("Keylength %d: Found key #%d: %c // %.2x\n", keyLength, i, trialKey, trialKey);
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
	

	//Cleanup stuff
	for (int i = 0; i < keyLength; i++)
		free(list_ptrsToBlocks[i]);
	free(list_ptrsToBlocks);


	if (foundKey == false)
	{
		if (debugging_mode)
			printf("No key found for keylength %d\n", keyLength);
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
* @param  inputString         /
* @param  inputLength         /
* @param  outputStringAddress /
* @return                     Length of output string
*/
int base64toBase256(char *inputString, int inputLength, char **outputStringAddress)
{
	//Initializations
	if (inputLength % 4 != 0)
	{
		fprintf(stderr, "Error[base64toBase256]: Passed an invalid input string, does not translate correctly into an output)");
		exit(1);
	}
	int outputLength = (inputLength / 4) * 3;
	char *outputString = (char *)malloc(sizeof (char)* (outputLength + 1));
	if (outputString == NULL)
	{
		fprintf(stderr, "Error[base64toBase256]: Memory allocation problems");
		exit(1);
	}
	int idx_output = 0;
	int idx_input = 0;


	for ( ; idx_input < inputLength; idx_input += 4, idx_output += 3)
	{
		outputString[idx_output] = (charToBase64(inputString[idx_input]) << 2) + (charToBase64(inputString[idx_input + 1]) >> 4);
		outputString[idx_output + 1] = (charToBase64(inputString[idx_input + 1]) << 4) + (charToBase64(inputString[idx_input + 2]) >> 2);
		outputString[idx_output + 2] = (charToBase64(inputString[idx_input + 2]) << 6) + charToBase64(inputString[idx_input + 3]);
	}

	//Adjusting size for padding
	if (inputString[idx_input - 2] == '=')
		outputLength -= 2;
	else if (inputString[idx_input -1] == '=')
		outputLength--; 

	outputString[outputLength] = '\0';

	//Outputs
	*outputStringAddress = outputString;
	return outputLength;
}

/**
* Some hacked together code
* Finds the top ten floats in the list, and puts them in order
* @param inputString         /
* @param inputLength         /
* @param outputStringAddress /
*/
void findTopTenKeyLengths(float *inputString, int inputLength, int **outputStringAddress)
{
	int *outputString = (int *)malloc(sizeof (int)* 10);
	float *temp = (float *)malloc(sizeof (float)* inputLength);
	memcpy(temp, inputString, sizeof(float)* inputLength);
	for (int i = 0; i < 10; i++)
	{
		float min = temp[0];
		float max = temp[0];
		int idx_min = 0;
		for (int j = 0; j < inputLength; j++)
		{
			float currentFloat = temp[j];
			if (currentFloat < min)
			{
				min = currentFloat;
				idx_min = j;
			}
			if (currentFloat > max)
				max = currentFloat;
		}
		outputString[i] = idx_min;
		temp[idx_min] = max;
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

        //stricter testing parameters
		///////////////////////////
		if (tester == 37)  //%
			return -1;
		if (tester == 126) //~
			return -1;
		if (tester == 35)  //#
			return -1;
		if (tester == 42)  // *
			return -1;
		//even stricter testing 
		///////////////////////////
		/*
		if (tester == 92) // backslash
			return -1;
		if (tester == 93) // ]
			return -1;
		if (tester == 94) // ^
			return -1;
		if (tester == '_')
			return -1;
		if (tester == '`')
			return -1;
			*/
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
