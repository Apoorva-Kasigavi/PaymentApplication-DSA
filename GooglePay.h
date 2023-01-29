#ifndef GOOGLEPAY_H_INCLUDED
#define GOOGLEPAY_H_INCLUDED

// Including all the required header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<math.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include<conio.h>


// Variable to hold the account data containing the
// account details
char ***account;


// Variable to hold the sorted account data in ascending
//order containing the account details
char ***accountHeapSorted;

// Variable to capture the time details
time_t mytime;


// Keeps a log of activity. Any errors can be checked in this file
FILE *flog;



// Function Declarations
int load();
int dump();
void open_log();
void close_log();
int account_memory_allocation();
void print_formatted();
int add_account();
void Transaction();
int fetchAccountIndex(char mobno[]);
void displayProfilesbyMobNumber(char pat[]);
void displayProfilesbyName(char pattern[]);
void write_log(char * function_name, char *status, char * message);
int file_empty_check(char *filename);
void account_memory_deallocation();
void pay(char mobno1[], char mobno2[],int index1);
void request(char mobno1[], char mobno2[], int index1);
void askMode(char mobno1[], char mobno2[], int index);
void prefixSuffixArray(char* pat, int M, int* pps);
int KMP(char* text, char* pattern);
int checkequality(char str[],char pat[],int start1,int end1,int start2,int end2);
void displayProfilesbyMobNumber(char pat[]);
void displayCost(int dist[],int path[],char source[],char locat[][25],int index);
int minele(int dist[],int visited[],int n);
void fillDist(int dist[],int visited[],int path[],int s, int n,int cost[][11]);
void dijkstras(int dist[],int path[],int visited[],int s,int n);
void displayLocations(char locat[][25]);
void initLocations(char locat[][25]);
int findIndex(char locat[][25], char loc[]);
void displayDistances();

void sortDecreasing();
void sortNames();
void displayBalance();





#endif // GOOGLEPAY_H_INCLUDED
