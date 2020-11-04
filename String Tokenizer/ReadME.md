String Tokenizer


Description:
The design behind our tokenizer.c application was to minimize extraneous if conditions and helper functions by utilizing the functionality of a Hash Map data structure. The program takes in a single string argument from argv[1] . The program dynamically allocates space for the input string utilizing malloc() The program then creates and initializes the Hash Map. After calculating the input length, we iterate through the input string checking each character and its corresponding value in the Hash Map using our searchResult() function. Our program also utilizes various flags created to help distinguish between word, decimal, octal, hex, float and other conditions.

Special Functions:

int hashFunction(char* str) : This function is in charge of the hashing of the HashMap. It takes in one value, a string str and it adds up the ASCII values of every character in the string. After doing so it mods the value by our defined MAPSIZE value (100), returning an integer containing the hash of the input string.

char* search(HashMap* hm, char *str) : This function takes in a pointer to the HashMap and the string, It takes the inputted HashMap and after the hashing is complete it compares it to the str with the value attributes of each Node. If the str matches with the val attribute of Node, then it returns the name attribute of the Node. If there is no match then the function returns a NULL pointer.

HashMap* insert(HashMap* hm, char* val, char* name): This function intakes a pointer to a HashMap, a string val, and a string name. It takes the inputted HashMap, and hashes val, before inserting a new node with val and name set to the corresponding parameters. It then returns a pointer to the updated HashMap.

void printMap(HashMap* hm): This function takes in a pointer to the HashMap and prints it out.

HashMap* hashMapInit(HashMap* hm): 
This function intake a pointer to a HashMap. It inputs all of the value and name pairs for each token, i.e ("+=", "plus equals") into the given HashMap. It returns a pointer to the updated HashMap.

char* multiCharacterOperatorParse(HashMap* hm, char* inputString, int* currentIndex, int maxAdditionalCharacters, char* initSearch, char* buffer):
This function intakes a pointer to a HashMap, the user-inputted string at the call of the program, the current index pointing the current character in the user-inputted string being analyzed, the longest length of a C operator that begins with the current character being analyzed, the initial search result of the character being analyzed and a pointer to the string that points at the character being analyzed.
This function is designed to handle C operators that begin with the same character but are longer than one character. i.e. <, <<, <<=, <=. In the user inputted string, when encountering such as '<', it is difficult to know exactly what the operator is considering there are so many options. This function will handle such a case, and output a string containing the name of the token. It will also update the buffer variable used in the main loop to contain the correct value of the token as well as update the counter variable, i, in the main loop so that it is pointing at the next character to be analyzed.

char* varCharParse(char* delimiter, char* buffer, int* currentIndex, char* inputString):
This function takes in a string delimiter that contains the ending sign to look for in the string, a pointer to the buffer variable in the main loop, a pointer the counter variable, i, in the main loop, and a pointer to the user-inputted string.
In C, there are strings with single quotes, double quotes, and multiple line comments. All of these can be and number of characters long, and they only end after seeing a specific character, or a delimiter. This function handles such cases, and returns the value of the string or comment, i.e. "hello" -> hello, 'hello' -> hello, /*hello -> hello. This function will also update the counter variable in the main loop, i, to point at the correct next character to analyze.

Challenges Faced:

1. Picking the most affective data structure to use that didn’t depend on an unknown length of data
At the beginning of this project, there seemed to be an overwhelming number of operators that we needed to account for, especially if we wanted to do the extra credit of including C keywords. In order to combat this, and the inevitable countless if statements that followed, we implemented a HashMap. This way, we could just search for a word or operator as it came up, and immediately get the token without having to write multiple equality checks. It also helped in debugging the code, because there were less if statements in the way of the actual logic in the code.

2. Accounting for special characters and conditions that can also be considered bash commands
The way we handled this challenge is through testing out exactly how specific bash commands show up when storing a string in C. Especially when dealing with trying to do double quote strings for the extra credit, we needed to find out just exactly how the inputs and outputs worked.

3. Accounting for all test cases and slight changes in when certain characters take precedence over others.
To combat this problem, we set up a function called multiCharParse. This function allowed us to differentiate between operators like '+' and '+='. It helped us so that we could get the longest token for a character that is that start to the multiple length operators.
