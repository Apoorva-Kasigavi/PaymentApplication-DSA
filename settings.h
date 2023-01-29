#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED

// These variables will be initialized from the files
// which maintains the count of users and contact List
int user_count = 0;
int key_count = 6;

// Memory allocated to the account table will be as per following:
// 1. user_count + value initialized for user_size for accounts
// 2. Maximum length of each word in table as set in word_length variable.
int user_size = 20;
int key_size = 6;
int word_length = 25;

//Structure to hold the data to be considered for sorting
struct Sort
{
    float amt;
    char MobNum[25];
};
typedef struct Sort SORT;


//Structure to hold the required data from the accounts table
//to sort the data in accounts table according to the names of the users.
struct tree
{
    char name[30];
    char MobNum[25];
    struct tree *left;
    struct tree *right;
};

typedef struct tree TREE;




// Flog file pointer is used to write the log files
FILE *flog;




//Structure to map the Locations with 0-based indexes
struct Location
{
    int ind;
    char l[25];
};
typedef struct location LOCATION;


//Below mentioned are to calculate hash value by Rolling Hash Method
#define prime 115
#define ll long long int
#define numberLocations 11

// Below mentioned are error status/status codes
#define SUCCESS                     1
#define FAILURE                     -1
#define FILE_OPEN_ERROR             1001
#define MEMORY_ALLOCATION_ERROR     1002
#define FILE_DELETE_ERROR           1003
#define COUNT_EXCEED_ERROR          1004
#define RESULT_NOT_FOUND            1005
#define FILE_EMPTY_ERROR            1006

#endif // SETTINGS_H_INCLUDED
