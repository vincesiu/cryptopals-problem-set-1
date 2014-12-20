// Vincent Siu
// 2014-12-19
// Basic file stuff which just makes reading the text from files and 
// putting it into a certain array really easy! 

#include <cstdio>
#include <cstdlib>

/**
 * Reads text input from a certain file into the CString provided by the user
 * Usage: inputLength = readTextFromFile (&destinationCSTRING, fileName);
 * @param  ptrToCString Address of Character Pointer, 
 * @param  fileName     C String of name of the target input file
 * @return              Length of the cstring read from file
 */
int readTextFromFile(char **ptrToCString, char *fileName)
{
	//Initialization of values and error checking of whatnot
	////////////////////////////////////////////////////////
	char *inputString = *ptrToCString;
	if (inputString != NULL)
	{
		fprintf(stderr, "Error[readTextFromFile]: Incorrect usage of arguments");
		exit(1);
	}


	FILE *inputFile;
	if (fopen_s (&inputFile, fileName, "r") != 0 || inputFile == NULL)
	{
		fprintf(stderr, "Error[readTextFromFile]: could not read from file");
		exit(1);
	}


	int inputLength = 0;
	char *tempPtr = NULL;
	inputString = (char *) malloc ( sizeof(char) );
	if (inputString == NULL)
	  {
	  fprintf(stderr, "Error[readTextFromFile]: memory allocation problems.");
	  return 1;
	  }

	//Reading input from the file using super safe TECHNOLOGY
	////////////////////////////////////////////////////////
	while (1)
	{
		int charHold = fgetc(inputFile);
		if ( charHold == EOF )
		{
			inputString[inputLength] = 0;
			break;
		}
		inputString[inputLength] = charHold;
		inputLength++;
		tempPtr = inputString;
		inputString = (char *) realloc ( tempPtr, sizeof(char) * (inputLength + 1));
		if (inputString == NULL)
		  {
	  		fprintf(stderr, "Error[readTextFromFile]: memory allocation problems.");
	  		return 1;
	  	  }
	}



	fclose (inputFile);
	*ptrToCString = inputString;
	return inputLength;
	}


/**
 * Given a CString, writing the text to the file (and overwriting anything that was previously inside the file)
 * @param  outputString Cstring with the text that you want to write
 * @param  fileName     name of the file you want to write output to
 */
void writeTextToFile(char *outputString, char *fileName)
{
	//Error checking
	////////////////////////////////////////////
	if (outputString == NULL)
	{
		fprintf(stderr, "Error[readTextFromFile]: Incorrect usage of arguments");
		exit(1);
	}


	FILE *outputFile;
	if (fopen_s (&outputFile, fileName, "w") != 0 || outputFile == NULL)
	{
		fprintf(stderr, "Error[readTextFromFile]: could not read from file");
		exit(1);
	}


	//Wow, writing is a lot easier than reading lol
	//////////////////////////////////////////////////////////
	fputs (outputString, outputFile);
	fclose (outputFile);
}