#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>

#define CHANGE_RED printf("\033[0;31m");
#define CHANGE_YELLOW printf("\033[0;33m");
#define CHANGE_GREEN printf("\033[0;32m");
#define CHANGE_CYAN printf("\033[0;36m");
#define CHANGE_BLUE printf("\033[0;34m");
#define RESET_COLOR printf("\033[0m");

typedef struct _wordNode {
    char* text;
    int occurrence;
    float probability;
    struct _wordNode* next;
} wordNode;

typedef struct _fileNode {
    char* path;
    int wordCount;
    struct _fileNode* next;
    wordNode* wordList;
    float kld;
} fileNode;

typedef struct _meanConstruction {
    char* text;
    float mean;
    struct _meanConstruction* next;
} meanConstruction;

//struct construct for thread initilization
typedef struct _threadArg {
    char* path;
    pthread_mutex_t* lock;
    fileNode* flist;
} threadArg;

wordNode* makeWord(char* token);
void printWL(wordNode* wn);
void swapWords(wordNode* w1, wordNode* w2);
void removeChar(char* str, char garbage);
void addToken(fileNode* file, char* token);
char* readInFile(fileNode* file);
void printList(fileNode *head);
void printMeanList(meanConstruction *head);
meanConstruction* makeMean(char* text,float mean);
void getColor(float jsd);
void cleanMeanList(meanConstruction* meanList);
void getJensenProb(fileNode* file, fileNode* fn);
float getKLD(fileNode* file, meanConstruction* mean);
void* directory_handling( void* arg );
void addProbabilities(fileNode* currentFile);
fileNode* makeFile(char* path);
void swapFiles(fileNode* f1, fileNode* f2);
void *file_handling(void* arg);
void cleanList(fileNode* fileList);
void fileAnalysis(fileNode* flist);

//constructs and allocates memory for a new wordNode
wordNode* makeWord(char* token){
    wordNode* newWord = (wordNode*)malloc(sizeof(wordNode));
    newWord->occurrence = 1;
    char* wtext = (char*)malloc(sizeof(char) * strlen(token) + 1);
    strcpy(wtext, token);
    newWord->text = wtext;
    newWord->next = NULL;
    return newWord;
}
//prints a wordlist for debugging
void printWL(wordNode* wn){
    while(wn != NULL){
        printf("\t\t%s\toccurrances:%d\tprob:%f\n", wn->text, wn->occurrence, wn->probability);
        wn = wn->next;
    }
}
//swaps the position of two wordNodes in a linked-list
void swapWords(wordNode* w1, wordNode* w2){
    wordNode temp = *w1;
    *w1 = *w2;
    *w2 = temp;
    w1->next = w2;
}
//removes the specified character in a string
void removeChar(char* str, char garbage){
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}
//adds a token to a file's wordlist
void addToken(fileNode* file, char* token){
    //discard empty tokens or tokens containing only spaces
    if (isspace(token[0]) || token[0] == '\0') return;
    
    removeChar(token, '\'');

    ++file->wordCount;
    
    if(file->wordList == NULL){
        //first word
        file->wordList = makeWord(token);
    } else {
        wordNode* last = file->wordList;
        while(last->next != NULL){
            //increment occurrence if already in list
            if(strcmp(last->text, token) == 0){
                ++last->occurrence;
                break;
            }
            //create new word and swap if list is out of order
            if(strcmp(last->text, token) > 0){
                wordNode* newNode = makeWord(token);
                swapWords(last, newNode);
                break;
            }
            last = last->next;
        }
        if(last->next == NULL){
            //checking last element in the list and swapping if applicable
            if(strcmp(last->text, token) == 0){
                ++last->occurrence;
            } else if(strcmp(last->text, token) > 0){
                wordNode* newNode = makeWord(token);
                swapWords(last, newNode);
            } else {
                last->next = makeWord(token);
            }
        }
    }
}
//reads file in and sets it to a buffer
//assumes that tokens will not be larger than 99 characters
char* readInFile(fileNode* file)
{
    int fd = open(file->path,O_RDONLY);
    if(fd == -1){
        printf("Cannot open the file %s\n", file->path);
        pthread_exit(NULL);
    }
    int fileSize = lseek(fd, 0, SEEK_END);
    //setting cursor to start of file
    lseek(fd, 0, SEEK_SET);
    char buffer[100];
    buffer[0] = '\0';
    char c;
    int status;
    int wordLen = 0;
    int charCount = 0;
    do {
        status = read(fd, &c, 1);
        if(status > 0){
            ++charCount;
            //discard punctuation that is not part of tokens
            if(ispunct(c) && c != '\'' && c != '-'){
                continue;
            }
            //make all chars lowercase
            c = tolower(c);
            //add the character to the current buffer
            buffer[wordLen] = c;
            buffer[wordLen + 1] = '\0';
            ++wordLen;
            //printf("buffer currently:'%s'\n", buffer);
            //if the character is a space then then token has ended and we process the token
            if(isspace(c) || isdigit(c) || charCount == fileSize){
                //discard the char that terminated the word
                if(isspace(c) || isdigit(c)){
                    buffer[wordLen-1] = '\0';
                }
                addToken(file, buffer);
                //prepare for the next word
                wordLen = 0;
                buffer[0] = '\0';
                
            }
        }
    } while (status > 0);
    close(fd);
    return NULL;
}
//prints out a filelist and each files wordlist
void printList(fileNode *head){   
    //first entry in the list is NULL
    fileNode *ptr=head;
        while(ptr != NULL){   
            printf("\t%s\ttotal tokens:%d\n",ptr->path, ptr->wordCount);
            //print out wordlist
            wordNode* wn = ptr->wordList;
                while(wn != NULL){
                    printf("\t\t%s\toccurrances:%d\tprob:%f\n", wn->text, wn->occurrence, wn->probability);
                    wn = wn->next;
                }
            ptr=ptr->next;    
        }
}
//prints out a meanConstruction linked-list
void printMeanList(meanConstruction *head){   
    //first entry in the list is NULL
    meanConstruction *ptr=head;
        while(ptr != NULL){   
            printf("\t%s\tmean:%f\n",ptr->text, ptr->mean);
            ptr=ptr->next;    
        }
}
//constructs and allocates a new meanConstruction
meanConstruction* makeMean(char* text,float mean){
    meanConstruction* newMean = (meanConstruction*)malloc(sizeof(meanConstruction));
    newMean->text = text;
    newMean->next = NULL;
    newMean->mean = mean ;
    return newMean;
}
//sets color to ouput text 
void getColor(float jsd){
    if(jsd>=0 && jsd<=0.1){
        CHANGE_RED
        printf("%f ",jsd);
        RESET_COLOR;
    }else if(jsd>0.1 && jsd<=0.15){
        CHANGE_YELLOW
        printf("%f ",jsd);
        RESET_COLOR;
    }else if(jsd>0.15 && jsd<=0.2){
        CHANGE_GREEN
        printf("%f ",jsd);
        RESET_COLOR;
    }else if(jsd>0.2 && jsd<=0.25){
        CHANGE_CYAN
        printf("%f ",jsd);
        RESET_COLOR;
    }else if(jsd>0.25 && jsd<=0.3){
        CHANGE_BLUE
        printf("%f ",jsd);
        RESET_COLOR;
    }else if(jsd>0.3){
        printf("%f ",jsd);
    }
}
//deallocates memory allocated by a meanlist
void cleanMeanList(meanConstruction* meanList){
    meanConstruction* current = meanList;
    while(current != NULL){
        meanConstruction* prev = current;
        current = current->next;
        free(prev);
    }
}
//computes the Jensen-Shannon Distance between two files
void getJensenProb(fileNode* file, fileNode* fn){
    wordNode* wl1 = file->wordList;
    wordNode* wl2 = fn->wordList;
    if(wl2 == NULL){
        //if wl2 == NULL then wl1 == NULL, jsd = 0
        getColor(0.0);
        printf(" \"%s\" and \"%s\"\n",file->path,fn->path);
        return;
    }
    if(wl1 == NULL && wl2 != NULL){
        //construct a meanlist only of wl2
        meanConstruction* meanList = makeMean(wl2->text, (wl2->probability)/2);
        meanConstruction* m = meanList;
        if(wl2->next != NULL){
            wl2 = wl2->next;
        }
        while(wl2 != NULL){
            m->next = makeMean(wl2->text, (wl2->probability)/2.0);
            wl2 = wl2->next;
            m = m->next;
        }
        float j=getKLD(file,meanList);
        float f=getKLD(fn,meanList);
        float jsd = (f+j) / 2;
        getColor(jsd);
        printf(" \"%s\" and \"%s\"\n",file->path,fn->path);
        cleanMeanList(meanList);
        return;
    }
    //adding first node and creating list
    meanConstruction* meanList = makeMean(wl1->text, (wl1->probability)/2);
    meanConstruction* m = meanList;
    if(wl1->next != NULL){
        wl1 = wl1->next;
    }
    while(wl1 != NULL){
        m->next = makeMean(wl1->text, (wl1->probability)/2.0);
        wl1 = wl1->next;
        m = m->next;
    }
    //reset m pointer
    m = meanList;
    //checking first node of wl2
    while(m->next != NULL){
        if(strcmp(wl2->text, m->text) == 0){
            m->mean += (wl2->probability / 2.0);
            break;
        }
        m = m->next;
    }
    if(m->next == NULL){
        m->next = makeMean(wl2->text, (wl2->probability) / 2.0);
    }
    wl2 = wl2->next;
    while(wl2 != NULL){
        char* curToken = wl2->text;
        //iterate through meanlist and see if curToken is already there
        while(m->next != NULL){
            if(strcmp(curToken, m->text) == 0){
                //found token
                m->mean += (wl2->probability / 2.0);
                break;
            }
            m = m->next;
        }
        //checking last item in list
        if(m->next == NULL){
            if(strcmp(curToken, m->text) == 0){
                //found token
                m->mean += (wl2->probability / 2.0);
                break;
            } else {
                //end of list, add new node
                m->next = makeMean(curToken, (wl2->probability) / 2.0);
            }
        }
        wl2 = wl2->next;
    }
    float j=getKLD(file,meanList);
    float f=getKLD(fn,meanList);
    float jsd = (f+j) / 2;
    //JSD (prints out final analyis between two)
    getColor(jsd);
    printf(" \"%s\" and \"%s\"\n",file->path,fn->path);
    cleanMeanList(meanList);
}
//Gets KLD and printout analysis
float getKLD(fileNode* file, meanConstruction* mean){
        wordNode* wordList = file->wordList;
        meanConstruction* meanList=mean;
        float kld = 0;
          // loop through the meanList, computing the probability of each word in the wordlist
            while(meanList!=NULL){
                while(wordList != NULL){
                    if(strcmp(wordList->text, meanList->text) == 0){
                        kld += wordList->probability * log10(wordList->probability / meanList->mean);
                        break;
                    }
                    wordList = wordList->next;
                }
                wordList = file->wordList;
                meanList = meanList->next;\
            }
    return kld;
}
//does directory_handling
void* directory_handling( void* arg ){
        threadArg* args = (threadArg*)arg;
        char* start_path = args->path;
        pthread_mutex_t* lock = args->lock;
        fileNode* fileList = args->flist;
        free(arg);
        pthread_t threads[100];
        int threadCount = 0;
        struct dirent *pDirent;
        DIR *pDir;
        // Ensure we can open directory.
        pDir = opendir (start_path);
        if (pDir == NULL) {
            printf ("Cannot open directory '%s'\n", start_path);
            exit(0);
        }
        // Reading . and .. directories (throwing them away)
        readdir(pDir);
        readdir(pDir);
        // Process each entry.
        while ((pDirent = readdir(pDir)) != NULL) {
                int pathlen = strlen(start_path) + 1 + strlen(pDirent->d_name);
                char* new_path = (char*)malloc(sizeof(char) * pathlen);
                strcpy(new_path, start_path);
                strcat(new_path, pDirent->d_name);
                threadArg* threadArgs = (threadArg*)malloc(sizeof(threadArg));
                threadArgs->flist = fileList;
                threadArgs->lock = lock;
            if(pDirent->d_type == DT_DIR && pDirent->d_type != DT_REG ){
                strcat(new_path, "/");
                threadArgs->path = new_path;
                pthread_create(&threads[threadCount++], NULL, directory_handling, (void*)threadArgs);
            }else if(pDirent->d_type == DT_REG){
                threadArgs->path = new_path;
                pthread_create(&threads[threadCount++], NULL, file_handling, (void*)threadArgs);
            }
        }
        // done spawning threads, join them all
        int j;
            for(j = 0;j < threadCount; j++){
                pthread_join(threads[j] ,NULL);
        }
        closedir (pDir);
        free(start_path);
        pthread_exit(NULL);
}
//fills in probabilities for each work to occur on a file
void addProbabilities(fileNode* currentFile){
    wordNode* curWord = currentFile->wordList;
    while(curWord != NULL){
        curWord->probability = ((float)curWord->occurrence / (float)currentFile->wordCount);
        curWord = curWord->next;
    }
}
// Creates a new file Node
fileNode* makeFile(char* path){
    fileNode* newFile = (fileNode*)malloc(sizeof(fileNode));
    newFile->path = path;
    newFile->next = NULL;
    newFile->wordList = NULL;
    return newFile;
}
//Swaps files in the linked list if order is wrong
void swapFiles(fileNode* f1, fileNode* f2){
    fileNode temp = *f1;
    *f1 = *f2;
    *f2 = temp;
    f1->next = f2;
}
// does file_handling
void *file_handling(void* arg){
    threadArg* args = (threadArg*)arg;
    char* filePath = args->path;
    pthread_mutex_t* lock = args->lock;
    fileNode* fileList = args->flist;
    free(args);
    int fd = open(filePath,O_RDONLY);
    if(fd == -1) {
        printf("Cannot open: %s", filePath);
        pthread_exit(NULL);
    }
    fileNode* newFile = makeFile(filePath);
    readInFile(newFile);
    addProbabilities(newFile);
    //critical section, adding to main file list
    pthread_mutex_lock(lock);
    if(fileList->path == NULL){
        //first file in the list
        *fileList = *newFile;
        //filelist already has been malloc'd
        free(newFile);
    } else {
        fileNode* last = fileList;
        while(last->next != NULL){
            if(last->wordCount > newFile->wordCount){
                swapFiles(last, newFile);
                break;
            }
            last = last->next;
        }
        if(last->next == NULL){
            if(last->wordCount > newFile->wordCount){
                swapFiles(last, newFile);
            } else {
                last->next = newFile;
            }
        }
    }
    pthread_mutex_unlock(lock);
    //end of critical section
    pthread_exit(NULL);
}
//frees the filelist and each file's words
void cleanList(fileNode* fileList){
    //first element is NULL and gets freed at a later point
    fileNode* currentFile = fileList;
    while(currentFile != NULL){
        //free the wordlist of the current file
        wordNode* currentWord = currentFile->wordList;
        while(currentWord != NULL){
            wordNode* prev = currentWord;
            currentWord = currentWord->next;
            free(prev->text);
            free(prev);
        }
        free(currentWord);
        fileNode* prevFile = currentFile;
        currentFile = currentFile->next;
        free(prevFile->path);
        free(prevFile);
    }
}
// Does the comparisons with all files found
void fileAnalysis(fileNode* flist){
    if(flist == NULL || flist->next == NULL){
        //one file in list, nothing to compare to
        printf("not enough files to analyze\n");
        return;
    }
    fileNode* f1 = flist;
    fileNode* f2 = flist->next;
    while(f1 != NULL){
        while(f2 != NULL){
            //printf("computing: %s, %s\n", f1->path, f2->path);
            getJensenProb(f1, f2);
            f2 = f2->next;
        }
        f1 = f1->next;
        if(f1->next != NULL){
            f2 = f1->next;
        } else {
            break;
        }
    }
}
int main(int argc,char *argv[]){
        if (argc != 2) {
            printf ("Provide Directory Please\n");
            exit(0);
        }
        //Beginning of file list
        fileNode* flist = malloc(sizeof(fileNode));
        flist->path = NULL;
        pthread_mutex_t* lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(lock, NULL);
        char* startdir = (char*)malloc(sizeof(char) * strlen(argv[1]) + 2);
        strcpy(startdir, argv[1]);
        strcat(startdir, "/");
        threadArg* arg = (threadArg*)malloc(sizeof(threadArg));
        arg->path = startdir;
        arg->lock = lock;
        arg->flist = flist;
        pthread_t mainThread;
        pthread_create(&mainThread, NULL, directory_handling, (void*)arg);
        pthread_join(mainThread, NULL);
        fileAnalysis(flist);
        cleanList(flist);
        free(lock);
        return 0;
}
