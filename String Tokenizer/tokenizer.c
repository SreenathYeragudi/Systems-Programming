#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
*
* This is a variable that defines the size of the HashMap used to store all of the tokens.
*
*/
#define MAPSIZE 100

/*
*
* Enum of different possible classifications of different number inputs.
* Ex: 
*	number - 21341
*	hex - 0x10AF4031
*	octal - 0273465234
*	decimal/float - 3.14, 3.156e-10
*
*/

typedef enum numclass{
	number,
	hex, 
	octal, 
	floating
} Numclass;

/*
*
* Node is the basic structure of the hashmap. 
* Node stores two strings, val and name, and a pointer to another node like a linked list.
* Val stores the input for a specific token - i.e. +=
* Name stores the token name for an input - i.e. plus equals
*
*/

typedef struct Node{
	char* val; 
	char* name;
	struct Node* next;
} Node;

/*
*
* HashMap is an array of pointers to Nodes. 
* Each entry in the array can potentially be a linked list in order to handle collision (chaining)
* The size of the HashMap is the global above, MAPSIZE. 
*
*/

typedef struct HashMap{

	Node* map[MAPSIZE];

} HashMap;

/*
*
* This function is the hashing function behind the HashMap. 
* It intakes one value, a string str. 
* It adds up the ascii values of every character in the str and then mods by MAPSIZE. 
* It returns an integer containing the hash of the input. 
* 
*/

int hashFunction(char* str);

/*
*
* This function intakes a pointer to a HashMap and a string str.  
* It takes the inputted HashMap, and, aftering hashing, compares str with the val attribute of each Node. 
* If str matches with the val attribute of a Node, then it returns the name attribute of the Node. 
* If there is no match, then the function returns a NULL pointer. 
* 
*/

char* search(HashMap* hm, char* str);

/*
*
* This function intakes a pointer to a HashMap, a string val, and a string name.  
* It takes the inputted HashMap, and hashes val, before inserting a new node with 
* val and name set to the corresponding parameters.
* It returns a pointer to the updated HashMap. 
*
*/

HashMap* insert(HashMap* hm, char* val, char* name);

/*
*
* This function intake a pointer to a HashMap, and prints out the HashMap. 
*
*/

void printMap(HashMap* hm);

/*
*
* This function intake a pointer to a HashMap.
* It inputs all of the value and name pairs for each token, i.e ("+=", "plus equals")
* into the given HashMap. 
* It returns a pointer to the updated HashMap. 
*
*/

HashMap* hashMapInit(HashMap* hm);

/*
*
* This function intakes a pointer to a HashMap, the user-inputted string at the call of the program, 
* the current index pointing the current character in the user-inputed string being analyzed, the longest length 
* of a C operator that begins with the current character being analyzed, the inital search result of the character being analyzed
* and a pointer to the string that points at the character being analyzed. 
* 
* This function is desinged to handle C operators that begin with the same character but are longer than one character. i.e. <, <<, <<=, <=. 
* In the user inputted string, when encountering such as '<', it is difficult to know exaclty what the operator is considering there are so many
* options. This function will handle such a case, and output a string containing the name of the token. It will also update the buffer variable used 
* in the main loop to contain the correct value of the token as well as update the counter variable, i, in the main loop so that it is pointing at 
* the next character to be analyzed. 
*
*/

char* multiCharacterOperatorParse(HashMap* hm, char* inputString, int* currentIndex, int maxAdditionalCharacters, char* initSearch, char* buffer);

/*
*
* This function takes in a string delimiter that contains the ending sign to look for in the string, 
* a pointer to the buffer variable in the main loop, a pointer the counter variable, i, in the main loop, 
* and a pointer to the user-inputted string. 
*
* In C, there are strings with single quotes, double quotes, and multiple line comments. All of these can be 
* and number of characters long, and they only end after seeing a specific character, or a delmiter. This function
* handles such cases, and returns the value of the string or comment, i.e. "hello" -> hello, 'hello' -> hello, 
* /*hello -> hello. This function will also update the counter variable in the main loop, i, to point at the correct
* next character to analyze. 
*
*/


char* varCharParse(char* delimiter, char* buffer, int* currentIndex, char* inputString);



int main(int argc, char** argv){

	if (argc != 2){
		return EXIT_FAILURE;
	}

	char* input = (char*)malloc(strlen(argv[1]) + 1);
	strcpy(input, argv[1]);

	HashMap* hm = (HashMap*)malloc(sizeof(HashMap));
	hm = hashMapInit(hm);
	int i = 0, inputLength = strlen(input);
	char* buffer = (char*)malloc(sizeof(char));
	char* searchResult = NULL, *temp = (char*)malloc(sizeof(char));
	while( i < inputLength){
		while(isspace(input[i]) && i < inputLength){
			i++;
		}
		if (i >= inputLength){
			break;
		}

		if(isalpha(input[i])){
			strcpy(buffer, "");
			while(isalpha(input[i]) || input[i] == '_' || isdigit(input[i])){
				// strcat(buffer, "%c", input[i]);
				strncat(buffer, input + i, 1);
				i++;
			}
			searchResult = search(hm, buffer);
			if (searchResult == NULL){
				printf("word: \"%s\"\n", buffer);
			}
			else{
				printf("%s: \"%s\"\n", searchResult, buffer);
			}
			i--;
		}

		else if(isdigit(input[i])){
			Numclass cur = number;
			strcpy(buffer, "");
			int counter = 0;
			int stillNum = (input[i] >= '0' && input[i] <= '9');
			int letterFlag = (tolower(input[i]) >= 'a' && tolower(input[i]) <= 'f' && cur == hex) ? 1 : -1;
			int eFlagCount = 0, prevEFlag = 0;
			int eFlag = (tolower(input[i]) == 'e' && cur != hex && eFlagCount <= 1 && prevEFlag) ? 1 : -1;
			int negCount = 0;
			int negFlag = ((input[i] == '-' || input[i] == '+') && cur == floating && prevEFlag == 1 && negCount <= 1) ? 1 : -1;
			int dotFlag = (input[i] == '.' && cur != hex && cur != floating) ? 1 : -1;
			int xFlagCount = 0;
			int xFlag = (tolower(input[i]) == 'x' && xFlagCount <= 1 && counter == 1 && cur == octal) ? 1 : -1;
			char prevChar = buffer[0];
            int prevdot=0;
			while(stillNum == 1|| letterFlag == 1 || eFlag == 1 || dotFlag == 1 || xFlag == 1 || negFlag == 1){
				if (counter == 0){
					if (input[i] == '0')
						cur = octal;
				}

				if (xFlag == 1){
					xFlag++;
					cur = hex;
				}

				if (negFlag == 1){
					
					negCount++;
				}

				if(dotFlag == 1){
					cur = floating;
				}

				if (eFlag == 1){
					eFlagCount++;
					cur = floating;
					
				}
				if(!(input[i] <= '7' && input[i] >= '0') && cur == octal){
					if(dotFlag == -1){
						cur = number;
					}
					else{
						cur = floating;
					}
				}

				strncat(buffer, input + i, 1);
				i++;
				counter++;

				//restting the flags
				stillNum = (input[i] >= '0' && input[i] <= '9') ? 1 : -1;
				letterFlag = (tolower(input[i]) >= 'a' && tolower(input[i]) <= 'f' && cur == hex) ? 1 : -1;
				prevEFlag = eFlag;
				eFlag = (tolower(input[i]) == 'e' && cur != hex && eFlagCount <= 1) ? 1 : -1;
                prevdot = dotFlag;
				dotFlag = (input[i] == '.' && cur != hex && cur != floating) ? 1 : -1;
				xFlag = (tolower(input[i]) == 'x' && xFlagCount <= 1 && counter == 1 && cur == octal) ? 1 : -1;
				negFlag = ((input[i] == '-' || input[i] == '+') && cur == floating && prevEFlag == 1 && negCount <= 1) ? 1 : -1;

			}
            if(prevdot == 1 || prevEFlag == 1){
				cur=number;
				i--;
				buffer[strlen(buffer)-1]= '\0';
			}

			switch(cur) {
				case number:
					printf("decimal integer: ");
					break;
				case floating:
					printf("floating point integer: ");
					if (eFlagCount == 2){
						i--;
						buffer[strlen(buffer) - 1] = '\0';
					}
					break;
				case hex:
					printf("hexedecimal integer: ");
					break;
				case octal:
					printf("octal integer: ");
					break;
			}
			i--;
			printf("\"%s\"\n", buffer);


		}

		else{
			strcpy(buffer, "");
			strncat(buffer, input + i, 1);
			//	^,  |. !, &, =, /. +, *, -, %
			if(strcmp(buffer, "%") == 0){
				i++;
				strncat(buffer, input + i, 1);
			}

			searchResult = search(hm, buffer);

			int flag1 = (strcmp(buffer, "^") == 0 || strcmp(buffer, "|") == 0 || strcmp(buffer, "!") == 0 || strcmp(buffer, "&") == 0) ? 1 : -1;
			int flag2 = (strcmp(buffer, "=") == 0 || strcmp(buffer, "/") == 0 || strcmp(buffer, "+") == 0 || strcmp(buffer, "*") == 0 || strcmp(buffer, "-") == 0) ? 1 : -1;

			if (strcmp(buffer, ">") == 0 || strcmp(buffer, "<") == 0){
				searchResult = multiCharacterOperatorParse(hm, input, &i, 2, searchResult, buffer);
			}
			else if (flag1 == 1 || flag2 == 1){
				searchResult = multiCharacterOperatorParse(hm, input, &i, 1, searchResult, buffer);
			}
			if(searchResult == NULL){
				printf("Unknown: \"%s\"\n", buffer);
			}
			else if (strcmp(buffer, "//") == 0){
				i++;
				printf("%s: \"%s\"\n", searchResult, input + i);
				i = strlen(input);
			}
			else if (strcmp(buffer, "/*") == 0 || strcmp(buffer, "\"") == 0 || strcmp(buffer, "'") == 0){
				strcpy(temp, "");
				if (strcmp(buffer, "/*") == 0)
					temp = varCharParse("*/", buffer, &i, input);
				else if (strcmp(buffer, "'") == 0)
					temp = varCharParse("'", buffer, &i, input);
				else 
					temp = varCharParse("\"", buffer, &i, input);
				printf("%s: \"%s\"\n", searchResult, temp);
			}
			else{
				printf("%s: \"%s\"\n", searchResult, buffer);
			}
		}

		i++;



	}
	
}



int hashFunction(char* str){
	int i = 0, hash = 0;
	for (i = 0; i < strlen(str); i++)
		hash += (int)str[i];
	return hash % MAPSIZE;
}

char* search(HashMap* hm, char* str){
	int index = hashFunction(str);
	Node* cur = hm->map[index];

	while (cur != NULL){

		if (strcmp(str, cur->val) == 0){
			return cur->name;
		}
		cur = cur->next;
	}
	return NULL;

}

HashMap* insert(HashMap* hm, char* val, char* name){
	int index = hashFunction(val);
	Node* cur = hm->map[index];
	Node* temp = (Node*) malloc(sizeof(Node)); 
	temp->val = val;
	temp->name = name;
	if (cur == NULL){
		hm->map[index] = temp;
		return hm;
	}
	while (cur->next != NULL){
		cur = cur->next;
	}
	cur->next = temp;
	return hm;
}

void printMap(HashMap* hm){
	int i = 0;
	Node* cur = NULL;
	for (i = 0; i < MAPSIZE; i++){
		cur = hm->map[i];
		if (cur != NULL){
			printf("%d: ", i);
			while(cur != NULL){
				printf("%s\t->\t", cur->val);
				cur = cur->next;
			}
			printf("\n");
		}
	}
}


HashMap* hashMapInit(HashMap* hm){
		//uniques - 1 character
	hm = insert(hm, "(", "left parenthesis");
	hm = insert(hm, ")", "right parenthesis");
	hm = insert(hm, "[", "left bracket");
	hm = insert(hm, "]", "right bracket");
	hm = insert(hm, ".", "structure member");
	hm = insert(hm, ",", "comma");
	hm = insert(hm, "?", "conditional true");
	hm = insert(hm, ":", "conditional false");
	hm = insert(hm, "~", "1s complement");

	//unqiues - 2 characters
	hm = insert(hm, "%=", "mod equals");

	//characters that start with - sign
	hm = insert(hm, "-", "minus/subtract operator");
	hm = insert(hm, "->", "structure pointer");
	hm = insert(hm, "--", "decrement");
	hm = insert(hm, "-=", "minus equals");

	//characters that start with *
	hm = insert(hm, "*", "multiply/dereference operator");
	hm = insert(hm, "*=", "times equals");

	//characters that start with +
	hm = insert(hm, "+", "addition");
	hm = insert(hm, "++", "increment");
	hm = insert(hm, "+=", "plus equals");

	//characters that start with /
	hm = insert(hm, "/", "division");
	hm = insert(hm, "/=", "divide equals");
	hm = insert(hm, "//", "single-line comment");
	hm = insert(hm, "/*", "multiple line comment");

	//characters that start with =
	hm = insert(hm, "=", "assignment");
	hm = insert(hm, "==", "equality test");

	//characters that start with &
	hm = insert(hm, "&", "AND/address operator");
	hm = insert(hm, "&=", "bitwise AND equals");
	hm = insert(hm, "&&", "logical AND");


    //character that start with backslash
    hm= insert(hm,"\\","backslash");

	//characters that start with !
    
	hm = insert(hm, "!", "negate");
	hm = insert(hm, "!=", "inequality test");

	//characters that start with |
	hm = insert(hm, "|", "bitwise OR");
	hm = insert(hm, "||", "logical OR");
	hm = insert(hm, "|=", "bitwise OR equals");

	//characters that start with >
	hm = insert(hm, ">", "greater than test");
	hm = insert(hm, ">>", "shift right");
	hm = insert(hm, ">=", "greater than or equal test");
	hm = insert(hm, ">>=", "shift right equals");

	//characters that start with <
	hm = insert(hm, "<", "less than test");
	hm = insert(hm, "<<", "shift left");
	hm = insert(hm, "<=", "less than or equal test");
	hm = insert(hm, "<<=", "shift left equals");

	//characters that start with ^
	hm = insert(hm, "^", "bitwise XOR");
	hm = insert(hm, "^=", "bitwise XOR equals");


	//C strings
	hm = insert(hm, "'", "single quote string");
	hm = insert(hm, "\"", "double quote string");

	//C keywords
	hm = insert(hm, "auto", "auto");
	hm = insert(hm, "break", "break");
	hm = insert(hm, "case", "case");
	hm = insert(hm, "char", "char");
	hm = insert(hm, "const", "const");
	hm = insert(hm, "continue", "continue");
	hm = insert(hm, "default", "default");
	hm = insert(hm, "do", "do");
	hm = insert(hm, "double", "double");
	hm = insert(hm, "else", "else");
	hm = insert(hm, "extern", "extern");
	hm = insert(hm, "float", "float");
	hm = insert(hm, "for", "for");
	hm = insert(hm, "goto", "goto");
	hm = insert(hm, "if", "if");
	hm = insert(hm, "int", "int");
	hm = insert(hm, "long", "long");
	hm = insert(hm, "register", "register");
	hm = insert(hm, "return", "return");
	hm = insert(hm, "short", "short");
	hm = insert(hm, "signed", "signed");
	hm = insert(hm, "sizeof", "sizeof");
	hm = insert(hm, "static", "static");
	hm = insert(hm, "struct", "struct");
	hm = insert(hm, "switch", "switch");
	hm = insert(hm, "typedef", "typedef");
	hm = insert(hm, "union", "union");
	hm = insert(hm, "unsigned", "unsigned");
	hm = insert(hm, "void", "void");
	hm = insert(hm, "volatile", "volatile");
	hm = insert(hm, "while", "while");
    return hm;
}


char* multiCharacterOperatorParse(HashMap* hm, char* inputString, int* currentIndex, int maxAdditionalCharacters, char* initSearch, char* buffer){
	int k;
	char *prevSearchResult = initSearch, *curSearchResult = NULL;

	for(k = 1; k <= maxAdditionalCharacters; k++){
		
		strncat(buffer, inputString + *currentIndex + k, 1);
		curSearchResult = search(hm, buffer);
		if (curSearchResult == NULL){
			
			buffer[strlen(buffer) - 1] = '\0';
			*currentIndex = *currentIndex + k - 1;
			return prevSearchResult;
		}
		prevSearchResult = curSearchResult;
	}
	
	*currentIndex = *currentIndex + k - 1;
	return prevSearchResult;
}

char* varCharParse(char* delimiter, char* buffer, int* currentIndex, char* inputString){


	(*currentIndex)++;
	char* str = (char*)malloc(1);
	strcpy(buffer, "");
	int boundsFlag = (((*currentIndex) + strlen(delimiter)) < strlen(inputString) + 1) ? 1 : -1;
	while(strcmp(buffer, delimiter) != 0 && boundsFlag == 1){
		strcpy(buffer, "");
		strncat(buffer, inputString + *currentIndex, strlen(delimiter));
		strncat(str, inputString + *currentIndex, 1);
		(*currentIndex)++;
		boundsFlag = boundsFlag = (((*currentIndex) + strlen(delimiter)) < strlen(inputString) + 1) ? 1 : -1;
	}
	if (boundsFlag == -1 && strcmp(buffer, delimiter) != 0){
		printf("Unfinished ");
	}
	else{
		str[strlen(str) - 1] = '\0';
	}
	if (strlen(delimiter) == 2 && boundsFlag == -1 && strcmp(buffer, delimiter) != 0){
		strncat(str, inputString + *currentIndex, 1);
	}
	if(strlen(buffer) == 1){
		(*currentIndex)--;
	}
	return str;
}

