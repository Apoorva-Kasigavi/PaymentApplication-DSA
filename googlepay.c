#include "googlepay.h"
#include "settings.h"





//Function:     open_log
//Description:  opens the log file to write the success or failure status
//Input param:  NULL
//Return Type:  NULL
void open_log()
{
    // Open the file
    flog = fopen("log.txt", "a+");


    // If unable to open the file, alert the user
    if(flog == NULL)
        printf("Unable to open log file. No log status will be captured.");
    else
        fprintf(flog, "%s", "_________________________START OF ITERATION________________________");

    fprintf(flog, "%s", "\n\n");
}


//Function:     close_log
//Description:  closes the log file if opened
//Input param:  NULL
//Return Type:  NULL
void close_log()
{
    if(flog != NULL) {
        // Add a message to indicate end of a iteration
        fprintf(flog, "%s", "_________________________END OF ITERATION________________________");
        fprintf(flog, "%s", "\n\n");

        // Close the file pointer
        fclose(flog);
    }
}


//Function:     write_log
//Description:  logs the status with success or failure message
//Input param:  function name, status and message to display
//Return Type:  NULL
void write_log(char * function_name, char *status, char * message)
{
    // Write the appropriate log message with associated run time
    mytime = time(NULL);
    fprintf(flog, "%s %s : %s --> %s\n\n", ctime(&mytime), function_name, status, message);

}


//Function:     file_empty_check
//Description:  Check whether file has content or not
//Input param:  name of the file
//Return Type:  integer
//              success state is returned if file has content
//              FILE_EMPTY_ERROR otherwise
int file_empty_check(char *filename)
{
    // Declaring "stat" a system call to know about content of file.
    struct stat filestat;
    stat (filename,&filestat);

    // Variables to hodl messages
    char message_failure[40];
    char message_success[40];

    strcpy(message_failure, filename);
    strcat(message_failure, " is empty");

    strcpy(message_success, filename);
    strcat(message_success, " has content");

    //check size of file and return appropriate status.
    if( filestat.st_size == 0 ){
        write_log("file_empty_check","FILE_EMPTY_ERROR",message_failure);
        return FILE_EMPTY_ERROR;
    }
    else {
        write_log("file_empty_check","SUCCESS",message_success);
        return SUCCESS;
    }
}


//Function:     account_memory_allocation
//Description:  dynamically allocates the memory to the account table
//Input param:  NULL
//Return Type:  integer
//              success status on successful memory allocation
//              MEMORY_ALLOCATION_ERROR on failure.
int account_memory_allocation()
{
    // Local variables
    int index1;
    int index2;

    // Calculate the required size for accounts.
    // We allocate the the current count plus some additional
    // for the new incoming values for the table.
    user_size = user_size + user_count;

    account = malloc(user_size * sizeof(char**));
    if(account == NULL)
        return MEMORY_ALLOCATION_ERROR;

    for(index1 = 0; index1 < user_size; index1++) {
        account[index1] = malloc(key_size * sizeof(char*));
        if(account[index1] == NULL)
            return MEMORY_ALLOCATION_ERROR;
        for(index2 = 0; index2 < key_size; index2++) {
            account[index1][index2] = malloc(word_length * sizeof(char));
            if(account[index1][index2] == NULL)
                return MEMORY_ALLOCATION_ERROR;
        }
    }
    write_log("account_memory_allocation", "SUCCESS", "Memory allocation Successful");
    return SUCCESS;
}



//Function:     load
//Description:  loads the account details from the respective file
//              loads the account table data from secondary memory to primary
//Input param:  NULL
//Return Type:  integer
//              success status on successful operation
//              respective error status code otherwise
int load()
{
    // Start the log file
    open_log();

    // Use a file pointer to open various files to load the values
    FILE *fp;

    //Local variables
    int index       = 0;
    int key_index   = 0;
    int status      = 0;
    int file_status = 0;

    //check whether user_count.txt file is empty or not.
    file_status = file_empty_check("user_count.txt");
    if (file_status == 1006)
        return FAILURE;

    //check whether account_file.txt file is empty or not
    file_status = file_empty_check("account_file.txt");
    if (file_status == 1006)
        return FAILURE;


    // Open the user_count file to read the number of accounts
    fp = fopen("user_count.txt","r");
    if(fp == NULL) {
        write_log("load", "FILE_OPEN_ERROR", "Unable to open the user_count file");
        return FAILURE;
    }

    fscanf(fp,"%d", &user_count);
    if (user_count <= 0) {
        write_log("load", "FAILURE", "user count is 0 or less than 0");
        return FAILURE;
    }
    write_log("load", "SUCCESS", "user count read successfully");
    fclose(fp);


    // Open the account_file to read the available account details
    fp = fopen("account_file.txt", "r");
    if (fp == NULL) {
        write_log("load", "FILE_OPEN_ERROR", "Error in opening the account_file");
        return FAILURE;
    }

    // Allocate the memory for account table
    status = account_memory_allocation();

    if(status ==  1002) {
        write_log("load", "MEMORY_ALLOCATION_ERROR", "No memory for account table");
        return FAILURE;
    }

    // Load the details from file to main memory
    while(!feof(fp)) {
        for(index = 0; index < key_count; index++) {
            fscanf(fp, "%s ", account[key_index][index]);
         }
        key_index++;
    }
    fclose(fp);



    write_log("load", "SUCCESS", "Account Load Successful");
    return SUCCESS;
}






//Function:     add_account
//Description:  Registers a new account to Google Pay
//Input param:  NULL
//Return Type:  integer type
//              success status is returned if new account is successfully
//              added to account file
//              failure status otherwise
int add_account()
{
   // Local variables
    FILE *fp;
    FILE *f;

    char GooglePin[5];

    char name[20];
    char mobileNumber[20];
    char mobno[20];
    char accNumber[20];
    char UPI_Pin[5];
    int pin;
    int OTP;
    float balance;
    int status;
    int flag = 0;


    int result;



    //Asking the user to enter Google Pin which will further be used
    //to use Google Pay Application
    printf("\n-----Welcome to Google Pay-----\n");
    printf("\n--Enter Google Pin [4 digits]\n");
    scanf("%s",GooglePin);

    while(!(atoi(GooglePin)>999 && atoi(GooglePin)<10000))
    {
        printf("Invalid, Google Pin should be 4 digits only\n");
        printf("\n--Enter Google Pin [4 digits]--\n");
        scanf("%s",GooglePin);
         printf("\n");
    }



    printf("\n--Enter your Mobile Number (From bank_file.txt)\n");
    fflush(stdin);
    scanf("%s",mobileNumber);


   result = strtol(mobileNumber,0,10);
  // printf("%d\n",result);
   while(!(result>999999999 && result<10000000000))
    {
     printf("Invalid Mobile Number\n");
     printf("\n--Enter your Mobile Number (From bank_file.txt)\n");
     scanf("%s",mobileNumber);
     result = strtol(mobileNumber,0,10);
    }

     status = fetchAccountIndex(mobileNumber);


     if(status != -1)
     {
         printf("%d\n",status);
         printf("The account is already registered with Google Pay\n");
         return FAILURE;
     }
     printf("\n...........................\n");
    //OTP Generator to verify the Mobile Number


    srand( time(NULL) );
    int randomID = 1000+(rand()%9000);
    fp = fopen("otp.txt", "w");




    fprintf(fp,"%d",randomID);
    fflush(fp);



    printf("Enter the OTP sent to your mobile number.(otp.txt file)\n");

    scanf("%d",&OTP);

    if(randomID != OTP)
    {
        printf("Invalid OTP\n");
        // Write the FAILURE log
        write_log("add_account", "FAILURE", "The provided mobile number was not valid");
        return FAILURE;
    }

    fclose(fp);
    // Delete the file as, it is no more required
    status = remove("otp.txt");
    if(status == 0 )
        write_log("add_account", "SUCCESS", "The OTP file was successfully deleted");
    else
        write_log("add_account", "FILE_DELETE_ERROR", "The OTP file was not deleted. Requires manual delete.");


    f = fopen("bank_file.txt", "r");

    while(!feof(f))
    {
       fscanf(f,"%s %s %s %d %f\n",name,mobno,accNumber,&pin,&balance);
       if(strcmp(mobileNumber,mobno)==0)
       {
           flag = 1;
           break;
       }
    }

    fclose(f);

    if(flag == 0)
    {
        printf("\n%s is not activated with Net Banking\n",mobileNumber);
        printf("Please contact your bank and activate net banking for Google Pay to work.\n");
        //Details of non existing accounts with Net Banking should be filled manually into the
        //bank_file.txt because the application here is not considering the working of a Bank.
        printf("[Enter the Bank details in bank_file.txt manually]");
        return FAILURE;
    }


    printf("\n...........................\n");
    printf("Enter UPI Pin[4 digits]\n");
    scanf("%s",UPI_Pin);

    while(!(atoi(UPI_Pin)>999 && atoi(UPI_Pin)<10000))
    {
        printf("\nInvalid UPI Pin\n");
        printf("Enter UPI Pin[4 digits]\n");
        scanf("%s",UPI_Pin);
    }

    // Increment the user count as the new account will be added
    user_count++;

    printf("\n...........................\n");
    //Add the new account details to account table

    strcpy(account[user_count-1][0],GooglePin);
    strcpy(account[user_count-1][1],name);



    strcpy(account[user_count-1][2],mobileNumber);


    strcpy(account[user_count-1][3],accNumber);


    strcpy(account[user_count-1][4],UPI_Pin);

    sprintf(account[user_count-1][5],"%f",balance);



    // Write the success log and close the file
    write_log("add_account", "SUCCESS", "The account details were successfully added");
    printf("Account Successfully Created\n");

    return SUCCESS;
 }




// Function:     print_formatted
// Description:  displays all the accounts' details
//               formatting will look cleaner even if the table
//               size is large
// Input param:  NULL
// Return Type:  NULL
void print_formatted()
{
        // Local variables and maximum sizes
        int i = 0;
        int j = 0;
        int max = 0;
        int temp_len = 0;

        // Creates a table to store the maximum size of a string in a column
        int *max_size = (int *)malloc(sizeof(int) * key_count);
        if (max_size == NULL)
        {
            write_log("print_formatted", "MEMORY_ALLOCATION_ERROR", "No memory for max_size array");
            return;
        }

        // Stores the maximum size of a string in a column among other values
        for (i = 0; i < key_count; i++)
        {
            for (j = 0; j < user_count; j++)
            {
                temp_len = strlen(account[j][i]);
                if (max < temp_len)
                    max = temp_len;
            }
            // Spacing to separate each column
            max_size[i] = (max + 4);
            max = 0;
        }

        printf("%-*s%-*s%-*s%-*s%-*s%-*s\n", max_size[0],"GPin", max_size[1],"Name",max_size[2],"MobNum",max_size[3],"AccNum",max_size[4],"UPI",max_size[5],"Balance");
        printf("----------------------------------------------------------------\n");
        // Run the loop and print the accounts table details with formatting
        for (j = 0; j < user_count; j++)
        {
            for (i = 0; i < key_count; i++)
            {
                // Adds right padding to the values appropriately
                printf("%-*s", max_size[i], account[j][i]);
            }
            printf("\n");
        }
        free(max_size);
}


//Function:     account_memory_deallocation
//Description:  free the allocated memory for account table
//Input param:  NULL
//Return Type:  NULL
void account_memory_deallocation()
{
        // Local variables
        int index1;
        int index2;

        for(index1 = 0; index1 < user_size; index1++) {
            for(index2 = 0; index2 < key_size; index2++) {
                free(account[index1][index2]);
            }
            free(account[index1]);
        }
        free(account);

        write_log("account_memory_deallocation", "SUCCESS", "Memory deallocation Successful");
}




//Function:     dump
//Description:  dumps the account table data from main to secondary memory
//Input param:  NULL
//Return Type:  integer
//              Success status on successful operation
//              otherwise respective error status code
int dump()
{
    // Local variables
    int i;
    int j;


    // File pointer
    FILE *fp;
    FILE *tfp;


    // Open the file in write mode and dump the latest user count
    fp = fopen("user_count.txt","w+");
    if(fp == NULL) {
        write_log("dump", "FILE_OPEN_ERROR", "Unable to open the user_count file");
        tfp = fopen("tempfile.txt", "a+");
        fprintf(tfp, "%s","\nFollowing contents to be added in said file");
        fprintf(tfp, "%s", "user_count.txt\n");
        fprintf(tfp,"%d", user_count);
        fclose(tfp);
        return FAILURE;
    }
    fprintf(fp,"%d", user_count);
    write_log("dump", "SUCCESS", "User count dumped successfully");
    fclose(fp);

    // Open the account file and update the values from the main memory
    // Into the secondary storage
    fp = fopen("account_file.txt", "w+");
    if (fp == NULL) {
        write_log("dump", "FILE_OPEN_ERROR", "Unable to open the account_file");
        tfp = fopen("tempfile.txt", "a+");
        fprintf(tfp, "%s","\nFollowing contents to be added in said file");
        fprintf(tfp, "%s", "account_file.txt\n");
        for (i = 0; i < user_count; i++) {
            for (j = 0; j < key_count; j++) {
                fprintf(tfp, "%s ", account[i][j]);
            }
            fprintf(tfp, "%s", "\n");
        }
        fclose(tfp);
        return FAILURE;
    }

    for (i = 0; i < user_count; i++) {
        for (j = 0; j < key_count; j++) {
            fprintf(fp, "%s ", account[i][j]);
        }
        fprintf(fp, "%s", "\n");
    }
    fclose(fp);

    // Free the memory for account
    account_memory_deallocation();



    write_log("dump", "SUCCESS", "Account table dumped successfully");

    // Close the log file
    close_log();
    return SUCCESS;
}




//Function:     fetchAccountIndex
//Description:  finds the index at which the given number is stored in
//              accounts table
//Input param:  mobile number
//Return Type:  integer
int fetchAccountIndex(char mobno[])
{
    int index = -1;

    for(int i=0 ; i < user_count ; i++)
    {
            //If the required mobile number and that in the accounts table is same
            //return the index
            if(strcmp(mobno,account[i][2])==0)
            {
                index = i;
                write_log("fetchAccountIndex", "SUCCESS", "Required Account Found");
                break;
            }

    }
    //If the required mobile number is not present in the accounts table
    //return the index as -1
    write_log("fetchAccountIndex", "FAILURE", "Required Account Not Found");
    return index;
}





//Function:     pay
//Description:  Transfers the amount from one account to another
//              provided that there are sufficient funds in the former account
//Input param:  Two mobile numbers to be taking part in the transaction
//              index of the first mobile number in the accounts table
//Return Type:  NULL
void pay(char mobno1[], char mobno2[],int index1)
{
    //Local variables
    float amount =0;
    char upi[4];
    int index2 = -1;


    printf("\n----------------------\n");
    printf("Enter the amount to be transferred");
    printf("\n----------------------\n");
    scanf("%f",&amount);


    while(amount <= 0)
    {
        printf("Enter amount greater than 0\n");
        scanf("%f",&amount);
    }


    //If the amount is greater than the amount in the bank account of the user1
    //exit the transaction
    if(atoi(account[index1][5]) < amount)
    {
        write_log("pay", "FAILURE", "Insufficient Funds");
        printf("Insufficient funds in your account\n");
        return;
    }


    //Find the index of the user2 in the accounts table
    index2 = fetchAccountIndex(mobno2);

    //If the index is -1, there is no Google Pay user with the
    //mobile number given by user1
    if(index2 == -1)
    {
        write_log("pay", "FAILURE", "Invalid Mobile Number");
        printf("Invalid Mobile Number\n");
        return;
    }


    printf("\n----------------------\n");
    printf("Enter the UPI Pin");
    printf("\n----------------------\n");
    scanf("%s",upi);

    //If the entered UPI Pin is not same as that of the pin associated with user1
    //present in accounts table, exit the transaction
    if(strcmp(account[index1][4],upi) != 0)
    {
        write_log("pay", "FAILURE", "Invalid UPI Pin");
        printf("Invalid UPI Pin\n");
        return;
    }


    printf("---Do not press any button until notified---\n");

    //Performing the arithmetic operations addition and subtraction
    //on the balance amount of the two users
    sprintf(account[index1][5],"%f",atof(account[index1][5]) - amount);
    sprintf(account[index2][5],"%f",atof(account[index2][5]) + amount);

     printf("\n------Transaction Successfully Complete------\n");

     write_log("pay", "SUCCESS", "Transaction Successfully Complete");
}





//Function:     request
//Description:  Requests money to the specified user
//Input param:  Mobile Numbers of the users to be taking part in the transaction
//              index of the user1 in accounts table
//Return Type:  NULL
void request(char mobno1[], char mobno2[], int index1)
{
    //Local Variables
    float amount = 0;
    char upi[4];

    char gpin[4];
    char flag[4];


    printf("\n-------------------------\n");
    printf("Enter the amount to be requested");
    printf("\n-------------------------\n");
    scanf("%f",&amount);

    while(amount <= 0)
    {
        printf("Enter amount greater than 0\n");
        scanf("%f",&amount);
    }

    //Interface below will be that of the user to whom the request has been sent

    printf("Payment request Sent\n");
    printf("\n-----------------------------------------------------------------------------\n");
    printf("\n-----------------------------------------------------------------------------\n");
    printf("**INTERFACE OF USER TO WHOM REQUEST HAS BEEN SENT**\n\n");


    //Fetching the index of the user2 from accounts table
    int index2 = fetchAccountIndex(mobno2);

    //index2 = -1 indicates that the entered number does not have
    //an account registered with Google Pay
    if(index2 == -1)
    {
        printf("Invalid Mobile Number\n");
        write_log("Request", "FAILURE", "Number not registered with Google Pay");
        return;
    }


    printf("\n--------------------\n");
    printf("Enter your Google Pin");
    printf("\n--------------------\n");

    scanf("%s",gpin);


    //If the entered Google Pin does not match
    //with the Google Pin registered in accounts table, print as invalid
    if(atoi(account[index2][0]) != atoi(gpin))
    {
        printf("Invalid Pin\n");
        write_log("Request", "FAILURE", "Invalid Google Pin");
        return;
    }

    //The user decides if he wants to pay or no
    printf("Enter YES if you want to pay %f to user %s\n",amount,mobno1);
    printf("Enter NO if you do not want to pay %f to user %s\n",amount,mobno1);
    fflush(stdin);
    scanf("%s",flag);


    if(strcasecmp(flag,"No") == 0)
    {
        printf("Transaction denied\n");
        write_log("Request", "FAILURE", "Transaction denied");
        return;
    }


    //If the balance in the account is less than the amount to
    //be transferred, print the appropriate message and exit the transaction
    if(atoi(account[index2][5]) < amount)
    {
        printf("Insufficient funds in your account\n");
        write_log("Request", "FAILURE", "Insufficient funds");
        return;
    }

    printf("\n----------------------\n");
    printf("Enter the UPI Pin");
    printf("\n----------------------\n");

    scanf("%s",upi);


    //If the enteredUPI Pin does not match
    //with the UPI Pin registered in accounts table, print as invalid
    if(atoi(account[index2][4]) != atoi(upi))
    {
        printf("Invalid UPI Pin\n");
        write_log("Request", "FAILURE", "Invalid UPI Pin");
        return;
    }

    printf("---Do not press any button until notified---\n");



    //Converting float data type to string
    //and storing the result in accounts table
    sprintf(account[index1][5],"%f",atof(account[index1][5]) + amount);
    sprintf(account[index2][5],"%f",atof(account[index2][5]) - amount);


    printf("\n------Transaction Successfully Complete------\n");
    write_log("Request", "SUCCESS", "Transaction Successfully Complete");


}





//Function:     askMode
//Description:  Asks the user if they want to do Payment or Request for money
//Input param:  Mobile Numbers of the users to be taking part in the transaction
//Return Type:  NULL
void askMode(char mobno1[], char mobno2[], int index)
{
    //Variable decision to hold the mode the user wants: Payment or Request
    int decision;
    printf("\n--------------------\n");
    printf("Enter your choice\n");
    printf("1.Pay\n2.Request");
    printf("\n--------------------\n");
    scanf("%d",&decision);

    if(decision == 1)
    {
        pay(mobno1,mobno2,index);

    }
    else if(decision == 2)
    {
        request(mobno1,mobno2,index);
    }
}





//Function:     Transaction
//Description:  Initiates the transaction by verifying Google Pin
//Input param:  NULL
//Return Type:  NULL
void Transaction()
{
    //Local Variables
    char Googlepin[4];
    char mobno[20];
    char mobno2[20];
    int index=-1;


    printf("\n--------------------\n");
    printf("Enter your Phone Number");
    printf("\n\nNote: In the actual Google Pay Application, \nMobile Number will be saved when account gets created the first time");
    printf("\n--------------------\n");
    scanf("%s",mobno);


   //Search for the mobile number from the accounts table
    for(int i=0 ; i < user_count ; i++)
    {
        for(int j=0; j< key_count; j++)
        {
            if(strcmp(account[i][j],mobno) == 0)
            {
                index = i;
                break;
            }
        }
    }

    //If mobile number is not found, it means that the number is not registered with google pay
    if(index == -1)
    {
        printf("Mobile number not found\n");
        printf("Enter 1 to Create account on Google Pay\n");
        write_log("Transaction", "FAILURE", "Number not registered with Google Pay");
        return;
    }

    printf("\n--------------------\n");
    printf("Enter your Google Pin");
    printf("\n--------------------\n");

    scanf("%s",Googlepin);


    //If the entered is not present in the accounts table
    //stop the transaction and indicate the failure.
    if(index == -1)
    {
        printf("Invalid Mobile Number\n\n");
        write_log("Transaction", "FAILURE", "Invalid Mobile Number");
        return;
    }


    //If the entered Pin does not match with the registered
    //Google Pin, stop the transaction and indicate the failure.
    if(atoi(account[index][0]) != atoi(Googlepin))
    {
        printf("Wrong Pin\n");
        write_log("Transaction", "FAILURE", "Wrong Pin");
        return;
    }


    printf("\n");



        printf("--Welcome--\n");
        printf("Enter the Mobile Number to/from which the transaction has to be done\n");
        scanf("%s",mobno2);

        if(strcasecmp(mobno,mobno2) == 0)
        {
            printf("\n......................\n");
            printf("You cannot perform transaction to your own account\n");
            write_log("Transaction", "FAILURE", "Two numbers to be taking part in transaction were the same");
            return;
        }


        //AskMode to find if the user wants to pay or request for money
        askMode(mobno,mobno2,index);



}




//Function:     prefixSuffixArray
//Description:  Constructs the Pie-Table
//Input param:  Pattern pat, Length of patter M, Array to store constructed Pie-Table pps
//Return Type:  NULL
void prefixSuffixArray(char* pat, int M, int* pps)
{

   int length = 0;
   pps[0] = 0;
   int i = 1;

   while (i < M) {
      if (pat[i] == pat[length]) {
         length++;
         pps[i] = length;
         i++;
      } else {
         if (length != 0)
         length = pps[length - 1];
         else {
            pps[i] = 0;
            i++;
         }
      }
   }

}



//Function:     KMP
//Description:  Uses the Pie-Table and tries to find a match between text and pattern
//              using Knuth-Morris-Pratt Algorithm
//Input param:  Text text, Pattern pattern
//Return Type:  status, 1 if pattern is found in the text, 0 otherwise
int KMP(char* text, char* pattern)
{
   int M = strlen(pattern);
   int N = strlen(text);
   int pps[M];
   prefixSuffixArray(pattern, M, pps);
   int i = 0;
   int j = 0;

   //While there are characters in the text execute
   while (i < N) {
      if (pattern[j] == toupper(text[i])) {
         j++;
         i++;
      }

      if (j == M) {
         return 1;
         break;
      }

      else if (i < N && pattern[j] != toupper(text[i])) {
         if (j != 0)
         j = pps[j - 1];
         else
         i = i + 1;
      }

   }
   return 0;
}



//Function:     displayProfilesbyName
//Description:  Searches the Name of the users
//              in accounts table to find the pattern the user specifies
//Input param:  Pattern to be searched
//Return Type:  NULL
void displayProfilesbyName(char pattern[])
{
    //Local variable "found" to store the status of occurrence of
    //pattern in the account table
    int found;

    //Local variable indicating the presence of the account
    //matching the pattern
    int fact = 0;

    fflush(stdin);

    //Converting all the characters in the pattern
    //to upper case in order to ignore case difference in pattern and text
    for(int k=0;pattern[k]!='\0';k++)
        pattern[k] = toupper(pattern[k]);

    //Looping through accounts table and searching for the occurrence
    for(int i=0;i<user_count;i++)
    {
        //KMP function determines if the pattern is found in the current text
        found = KMP(account[i][1],pattern);
        if(found == 1)
        {
            fact = 1;
            printf("%s %s\n",account[i][1],account[i][2]);
            write_log("displayProfilesbyName", "SUCCESS", "Account found including the required pattern");
            found = 0;
        }
    }

    //If there is no account that matches the required input.
    //Print the appropriate message
    if(fact == 0)
    {
        printf("No Contacts Found\n");
        write_log("displayProfilesbyName", "FAILURE", "Account not found which includes the required pattern");
    }
}



//Function:     calculateHash
//Description:  calculates the hash value using Rolling Hash method
//Input param:  String whose hash value is to be computed
//              Length of text whose Hash Value is to be calculated
//Return Type:  NULL
ll calculateHash(char str[],int m)
{
    ll result=0;
    for(int i=0;i<m;i++)
    {
        result += (ll)(str[i] * (ll)pow(prime,i));
    }
    return result;
}




//Function:     recalculatehash
//Description:  calculates the new hash value making use of previous hash value
//Input param:  String whose hash value is to be computed
//              Old Index i.e. the index that has to be now discarded for calculating new
//              hash value
//              New Index i.e. the index that has to be now newly considered for calculating
//              new hash value.
//              Length of text whose Hash Value is to be calculated
//Return Type:  NULL
ll recalculatehash(char str[],int oldindex,int newindex,ll oldhash, int patlength)
{

    ll newhash = (oldhash - str[oldindex]);
    newhash/=prime;
    newhash = newhash + (ll)(str[newindex] * (ll)pow(prime,patlength-1));
    return newhash;
}




//Function:     checkequality
//Description:  This function compares the string character by character to check if
//              they are the same.
//Input param:  Two strings str , pat which has to be compared,
//              start and the end indexes for str and pat
//Return Type:  if the two strings are same 1 is returned, else 0
int checkequality(char str[],char pat[],int start1,int end1,int start2,int end2)
{
    //If the length of two strings are not equal,
    //Then there is no chance of them being the same,hence return 0
    if(end1-start1 != end2-start2)
        return 0;

    //Loop till the end of both strings while checking
    //if each character in them are same
    while(start1<=end1 && start2<=end2)
    {
        //If there is a mismatch in the characters of two strings, return 0
        if(str[start1]!= pat[start2])
        {
            return 0;
        }
        start1++;
        start2++;
    }

    //When loop ends without any mismatch in the characters of the strings
    //return 1 as the strings str and pat will be same
    return 1;

}




//Function:     displayProfilesbyMobNumber
//Description:  Searches for the input subset of mobile number in the
//              accounts table, uses Rabin Karp Algorithm
//Input param:  pattern which has to be searched in the accounts table
//Return Type:  NULL
void displayProfilesbyMobNumber(char pat[])
{
    ll texthash=0,patternHash = 0;

    //Local variable indicating the presence of the account
    //matching the pattern
    int fact = 0;

    //calculating the hash value of the pattern
    patternHash = calculateHash(pat,strlen(pat));

   for(int k=0;k<user_count;k++)
   {
    texthash=0;
    //str will hold the mobile number of each account
    char str[25];
    strcpy(str,account[k][2]);

    //calculating the hash value of the text
    texthash = calculateHash(str,strlen(pat));
    int m = strlen(pat);
    int n = strlen(str);
    int flag;

    //while incrementing the index by 1 each time
    //compare the hash value of the pattern and text
    for(int i=0;i<=n-m;i++)
    {
        //if the hash value of text and pattern is equal
        //Compare the characters of text and pattern one by one
        if(texthash == patternHash)
        {
            flag = checkequality(str,pat,i,i+m-1,0,m-1);

            //flag = 1, indicates that the text and pattern are same
            if(flag==1)
            {
                fact = 1;
                printf("%s %s\n",account[k][1],account[k][2]);
                write_log("displayProfilesbyMobNumber", "SUCCESS", "Account found including the required pattern");
                break;
            }

        }

        if(i<n-m)
         texthash = recalculatehash(str,i,m+i,texthash,m);

    }

   }

    //If there is no account that matches the required input.
    //Print the appropriate message
    if(fact == 0)
    {
        printf("No Contacts Found\n");
        write_log("displayProfilesbyMobNumber", "FAILURE", "Account not found which includes the required pattern");
    }

}




//Function:     displayCost
//Description:  Displays the minimum cost to visit various locations
//              using various means like auto,bus,cab
//Input param:  distance arrya, path array from Dijkstra's Algorithm
//              source, Array locat mapping the locations to 0 based indexes
//              index of the source
//Return Type:  NULL
void displayCost(int dist[],int path[],char source[],char locat[][25],int index)
{
    //For each location other than the source
    //Print the minimum cost to visit the location
    // using various means like auto,bus,cab
    for(int i=0;i<numberLocations;i++)
    {
        if(i != index)
        {
           printf("\n.....\n");
           printf("* %s -> %s : %d Km\n",source,locat[i],dist[i]);
           printf("   Minimum Auto Rickshaw Charge: Rs %d\n",dist[i]*16);
           printf("   Minimum Bus Charge: Rs %0.2f\n",dist[i]*1.625);
           printf("   Minimum Cab Charge: Rs %d\n",dist[i]*5);
        }

    }
    write_log("displayCost", "SUCCESS", "Minimum Cost to reach each location displayed\n");
}





//Function:     minele
//Description:  Finds a closest next location from a particular location
//Input param:  distance array, visited array, number of locations
//Return Type:  index of closest next location from a particular location
int minele(int dist[],int visited[],int n)
{
    int minEle = INT_MAX;
    int index;
    for(int i=0;i<n;i++)
    {
        if(visited[i]==0)
            if(dist[i] < minEle)
            {
                index = i;
                minEle=dist[i] ;
            }

    }
    return index;
}




//Function:     fillDist
//Description:  Fills the distance while making sure the entry has the most
//              minimum cost possible
//Input param:  distance array, visited array,path array,source index
//              number of locations, cost  matrix
//Return Type:  NULL
void fillDist(int dist[],int visited[],int path[],int s, int n,int cost[][11])
{
    //Traverse each time finding the the most
    //minimum cost possible, and making the updates in distance
    //and path array
    for(int i=0;i<n;i++)
    {
        if(visited[i]==0)
        {
            if(dist[i] > dist[s] + cost[s][i])
            {
                path[i] = s;
                dist[i] = dist[s] + cost[s][i];

            }
       }

    }
}




//Function:     dijkstras
//Description:  single source multiple destination algorithm
//Input param:  distance array, path array,visited array,source index
//              number of locations
//Return Type:  NULL
void dijkstras(int dist[],int path[],int visited[],int s,int n)
{
    //Local Variables
    int N =0,u;

    //Cost matrix where each entry cost[i][j]
    //gives the distance in Km from location i to location j
    int  cost[11][11]= {
                       {0, 9, 20, 15, 10, 5, 4, 6, 8, 2, 4},
                       {9, 0, 2, 4, 6, 5, 3, 1, 2, 3, 4},
                       {20, 2, 0, 3, 2, 4, 2, 3, 6, 10, 8},
                       {15, 4, 3, 0, 4, 3, 5, 2, 1, 3, 4},
                       {10,6 , 2, 4, 0, 3, 2, 6, 7, 8, 4},
                       {5, 5, 4, 3, 3, 0, 10, 7, 4, 3, 2},
                       {4, 3, 2, 5, 2, 10, 0 ,4, 3, 5, 6},
                       {6, 1, 3, 2, 6, 7, 4, 0, 2, 4, 5},
                       {8, 2, 6, 1, 7, 4, 3, 2, 0, 7, 5},
                       {2, 3, 10, 3, 8, 3, 5, 4, 7, 0, 6},
                       {4, 4, 8, 4, 4, 2,6, 5, 5, 6,0},
                        };

    //Initializing the distance array with the
    //distance in Km from source to all other locations
    for(int i=0;i<n;i++)
        dist[i] = cost[s][i];

    //Finding the minimum possible distance among others
    u = minele(dist,visited,n);
    visited[s] = 1;


    //Condition N<n says to continue the while loop
    //until there are locations which are still unvisited
    while(N<n)
    {
      visited[u] = 1;
      N++;
      fillDist(dist,visited,path,u,n,cost);
      u = minele(dist,visited,n);
    }
   write_log("dijkstras", "SUCCESS", "Minimum Cost to reach each location found\n");

}


//Function:     displayLocations
//Description:  display all the locations
//Input param:  array locat
//Return Type:  NULL
void displayLocations(char locat[][25])
{
    for(int i=0;i<numberLocations;i++)
        printf(" %s\n",locat[i]);
}



//Function:     initLocations
//Description:  Maps the locations with indexes, 0-based indexing is followed
//Input param:  array locat
//Return Type:  NULL
void initLocations(char locat[][25])
{
    strcpy(locat[0] , "JayaNagar");
    strcpy(locat[1] , "VidyaNagar");
    strcpy(locat[2] , "BhavaniNagar");
    strcpy(locat[3] , "Keshwapur");
    strcpy(locat[4] , "RajNagar");
    strcpy(locat[5] , "Amargol");
    strcpy(locat[6] , "NavNagar");
    strcpy(locat[7] , "Shivagiri");
    strcpy(locat[8] , "AkshayPark");
    strcpy(locat[9] , "CBT");
    strcpy(locat[10] , "SilverTown");

    write_log("initLocations", "SUCCESS", "Locations Mapped\n");
}



//Function:     findIndex
//Description:  Finds the index in the locat array for the entered location
//Input param:  array locat, string loc
//Return Type:  NULL
int findIndex(char locat[][25], char loc[])
{
    for(int i=0;i<numberLocations;i++)
    {
       //If the location asked by the user and that
       //present in locat array, return that index
       if(strcasecmp(locat[i],loc) == 0)
            return i;
    }

    //If the asked location is not in the
    //locat array return -1
    return -1;
}



//Function:     displayDistances
//Description:  Displays the locations in the town/city
//              along with minimum cost to travel to those places
//              via auto,bus,cab
//Input param:  NULL
//Return Type:  NULL
void displayDistances()
{
    //Local Variables
    char locat[numberLocations][25];
    initLocations(locat);
    char loc[25];
    int s;


    printf("\n------------------------\n");
    printf("LOCATIONS");
    printf("\n------------------------\n");

    //Display the list of locations as a reference for the user
    displayLocations(locat);


    printf("\n------------------------\n");
    printf("\nEnter your location\n");
    fflush(stdin);
    scanf("%s",loc);

    //Finding the index of the specified location in the locat array
    s = findIndex(locat,loc);

    //Local Variables to be used by Dijkstra's Algorithm
    int n=numberLocations;
    int path[n];
    int dist[n];

    //Array to keep track of visited locations
    int visited[n];

    for(int i=0;i<n;i++)
        visited[i] = 0;
    for(int i=0;i<n;i++)
        path[i] = s;


    dijkstras(dist,path,visited,s,n);

    displayCost(dist,path,loc,locat,s);
    write_log("displayDistances", "SUCCESS", "Minimum cost to reach each location displayed\n");

}





// Function:     print_formattedheapSorted
// Description:  displays all the sorted accounts' details
//               formatting will look cleaner even if the table
//               size is large
// Input param:  Heap Sorted array a, consisting of balance amount and
//               mobile number of the user
// Return Type:  NULL
void print_formatted_heapSorted(SORT a[])
    {
        // Local variables and maximum sizes
        int i = 0;
        int j = 0;
        int max = 0;
        int temp_len = 0;
        float f;
        int flag = 0;

        // Creates a table to store the maximum size of a string in a column
        int *max_size = (int *)malloc(sizeof(int) * key_count);
        if (max_size == NULL)
        {
            write_log("print_formatted_heapSorted", "MEMORY_ALLOCATION_ERROR", "No memory for max_size array");
            return;
        }

        // Stores the maximum size of a string in a column among other values
        for (i = 0; i < key_count; i++)
        {
            for (j = 0; j < user_count; j++)
            {
                temp_len = strlen(account[j][i]);
                if (max < temp_len)
                    max = temp_len;
            }
            // Spacing to separate each column
            max_size[i] = (max + 4);
            max = 0;
        }
        printf("%-*s%-*s%-*s%-*s%-*s%-*s\n", max_size[0],"GPin", max_size[1],"Name",max_size[2],"MobNum",max_size[3],"AccNum",max_size[4],"UPI",max_size[5],"Balance");
        printf("----------------------------------------------------------------\n");



        // Run the loop and print the accounts table details with formatting
        for(int k = 0; k<user_count ; k++)
        {

            for (j = 0; j < user_count; j++)
           {

               sscanf(account[j][5], "%f", &f);
               //If the entry in account table is same as the one in array a, print it
               if(a[k].amt == f && (strcmp(account[j][2],a[k].MobNum)==0))
               {

                   for (i = 0; i < key_count; i++)
                  {
                    // Adds right padding to the values appropriately
                    printf("%-*s", max_size[i], account[j][i]);
                    flag = 1;
                 }
               }
               if(flag == 1)
               {
                  printf("\n");
                  flag = 0;
                  break;
               }

           }
        }

         printf("\n---------------------------------------\n");
         free(max_size);

}




//Function:     max_heapify
//Description:  Max heapifies the array by correcting the disruption caused by a element
//Input param:  Array a, number of elements n
//Return Type:  NULL
void max_heapify(SORT a[],int n)
{
        SORT v;
        int k,j,heap;
        v = a[0];
        k = 0;
        heap = 0;

        //until we have a heap
        //and parent has at least one child
        while((heap==0) && ((2*k+1)<=n))
        {

            //Note the index of the first child
            j = 2*k+1;

            //Find the largest of two children, if exists
            if(j<n)
                if(a[j+1].amt>a[j].amt)
                  j = j+1;

            //If not heap copy the child to parent
            if(v.amt >= a[j].amt)
            {
                heap = 1;
                break;
            }
            else
            {
                a[k]=a[j];
                k = j;
            }
        }
        a[k] = v;
}




//Function:     build_max_heap
//Description:  Max heapifies arranging all the elements in the array
//Input param:  Array a, number of elements n
//Return Type:  NULL
void build_max_heap(SORT a[],int n)
{
    //Local Variables
    SORT v;
    int j,k,heap;

    //Run the iterations from n/2 down to 0
    for(int i=floor(n/2);i>=0;i--)
    {
        //Note the index and value of the parent
        v = a[i];
        k = i;

        //Set Heap to false
        heap = 0;

        //Until we have a heap and parent has at least
        //one child
        while((heap==0) && ((2*k+1)<=n))
        {
            //Note the index of the first child
            j = 2*k+1;

            //Find the largest of two children , if exists
            if(j<n)
                if(a[j+1].amt>a[j].amt)
                  j = j+1;

            //If not heap, copy the child to parent
            if(v.amt >= a[j].amt)
            {
                heap = 1;
                break;
            }
            else
            {
                a[k]=a[j];
                k = j;
            }
        }
        a[k] = v;
    }
}



//Function:     swap
//Description:  Swaps two structure variables of structure SORT
//Input param:  Two structure variables of structure SORT to be swapped
//Return Type:  NULL
void swap1(SORT *a,SORT *b)
{
    SORT temp;
    temp = *a;
    *a = *b;
    *b = temp;

}



//Function:     heapSort
//Description:  Sorts the contents of the array a based on balance amount
//Input param:  Array a, number of elements n
//Return Type:  NULL
void heapSort(SORT a[],int n)
{
    //Creates a max heap
    build_max_heap(a,n);

    printf("\n-------\n");

    //While there are still unsorted elements
    while(n)
    {
       //Swap the first and last element
       swap1(&a[0],&a[n]);

       //Decrement the number of elements yet to be sorted
       n--;

       //Function to recreate max heap, in case of any disruption
       max_heapify(a,n);

    }

}




//Function:     fillContents
//Description:  Fills the array a with balance amount and
//              mobile number from accounts table, which will later
//              be inputted to get sorted
//Input param:  Array a
//Return Type:  NULL
void fillContents(SORT a[])
{
    //Loop through each element in accounts table and store
    //balance amount and mobile number into array a
    for(int i=0;i<user_count;i++)
    {
        sscanf(account[i][5], "%f", &a[i].amt);
        strcpy(a[i].MobNum,account[i][2]);
    }
}



//Function:     display
//Description:  Display the contents of the array a
//Input param:  Array a
//Return Type:  NULL
void display(SORT a[])
{
    for(int i=0;i<user_count;i++)
    {
        printf("---%f  ",a[i].amt);
        printf("---%s\n",a[i].MobNum);
    }
}



//Function:     sortIncreasing
//Description:  Sort the contents of the accounts table in non-decreasing order
//              based on balance amount temporarily
//Input param:  NULL
//Return Type:  NULL
void sortIncreasing()
{
   //Creating a array to hold balance amount and mobile number
   SORT a[user_count];

   //Populating the array with balance amount and mobile number from
   //accounts table
   fillContents(a);

   //Heap sort algorithm is used to sort the contents
   heapSort(a,user_count-1);

   //Prints all the account details while comparing the
   //mobile number from array a and that of accounts table
   print_formatted_heapSorted(a);

   write_log("sortIncreasing", "SUCCESS", "Contents of accounts are sorted in non-decreasing order based on balance amount\n");

}



//Function:     partition
//Description:   This function takes first element as pivot, places
//               the pivot element at its correct position in sorted
//               array, and places all smaller (smaller than pivot)
//               to left of pivot and all greater elements to right of pivot
//Input param:  Array a, left index, right index
//Return Type:  partition index
int partition1(SORT a[],int l,int r)
{
    //Indexes to traverse the array finding element greater, smaller
    //than the pivot
    int i = l;
    int j = r+1;

    //pivot, element to be placed at the right position
    SORT p = a[l];
    do
    {
        //Repeat until we find a element smaller than the pivot
        do
        {
            i++;
        }while(a[i].amt>p.amt);

        //Repeat until we find a element greater than the pivot
        do
        {
            j--;
        }while(a[j].amt<p.amt);


      //Swap the smaller and greater elements found
      swap1(&a[i],&a[j]);
    }while(i<j);

    //Undo the wrong swap
    swap1(&a[i],&a[j]);

    //swap the element in the partition position and the pivot element
    swap1(&a[l],&a[j]);


    //return the partition index
    return j;

}



//Function:     quicksort
//Description:  Implements the quicksort algorithm
//Input param:  array a, start index l, end index r
//Return Type:  NULL
void quicksort(SORT a[],int l,int r)
{
    //repeat while left index is less than right index
    if(l<r)
    {
    int s;

    //find the partition index
    s = partition1(a,l,r);

    //repeat the quicksort algorithm on left half of the partition index
    quicksort(a,l,s-1);

    //repeat the quicksort algorithm on right half of the partition index
    quicksort(a,s+1,r);
    }
}


// Function:     print_formattedquickSorted
// Description:  displays all the sorted accounts' details
//               formatting will look cleaner even if the table
//               size is large
// Input param:  array a
// Return Type:  NULL
void print_formatted_quickSorted(SORT a[])
{
        // Local variables and maximum sizes
        int i = 0;
        int j = 0;
        int max = 0;
        int temp_len = 0;
        float f;
        int flag = 0;

        // Creates a table to store the maximum size of a string in a column
        int *max_size = (int *)malloc(sizeof(int) * key_count);
        if (max_size == NULL)
        {
            write_log("print_formatted_heapSorted", "MEMORY_ALLOCATION_ERROR", "No memory for max_size array");
            return;
        }

        // Stores the maximum size of a string in a column among other values
        for (i = 0; i < key_count; i++)
        {
            for (j = 0; j < user_count; j++)
            {
                temp_len = strlen(account[j][i]);
                if (max < temp_len)
                    max = temp_len;
            }
            // Spacing to separate each column
            max_size[i] = (max + 4);
            max = 0;
        }

        printf("%-*s%-*s%-*s%-*s%-*s%-*s\n", max_size[0],"GPin", max_size[1],"Name",max_size[2],"MobNum",max_size[3],"AccNum",max_size[4],"UPI",max_size[5],"Balance");
        printf("----------------------------------------------------------------\n");

        // Run the loop and print the accounts table details with formatting
        for(int k = 0; k<user_count ; k++)
        {

            for (j = 0; j < user_count; j++)
           {

               sscanf(account[j][5], "%f", &f);
               //If the entry in account table is same as the one in array a, print it
               if(a[k].amt == f && (strcmp(account[j][2],a[k].MobNum)==0))
               {

                   for (i = 0; i < key_count; i++)
                  {
                    // Adds right padding to the values appropriately
                    printf("%-*s", max_size[i], account[j][i]);
                    flag = 1;
                 }
               }
               if(flag == 1)
               {
                  printf("\n");
                  flag = 0;
                  break;
               }

           }
        }

         printf("\n---------------------------------------\n");
         free(max_size);

}



//Function:     sortDecreasing
//Description:  Sort the contents of the accounts table in non-increasing order
//              based on balance amount temporarily
//Input param:  NULL
//Return Type:  NULL
void sortDecreasing()
{

   //Creating a array to hold balance amount and mobile number
   SORT a[user_count];

   //Populating the array with balance amount and mobile number from
   //accounts table
   fillContents(a);

   //Quick sort algorithm is used to sort the contents
   quicksort(a,0,user_count-1);

   //Prints all the account details while comparing the
   //mobile number from array a and that of accounts table
   print_formatted_quickSorted(a);

   write_log("sortDecreasing", "SUCCESS", "Contents of accounts are sorted in non-increasing order based on balance amount\n");
}




//Function:     insert_into_bst
//Description:  inserts a node into binary search tree according to alphabetical order
//Input param:  root node, name, mobile number to be stored in new node
//Return Type:  TREE node
TREE * insert_into_bst(TREE *root,char w[],char mob[])
{
    int i=0;

    //assigning memory to newnode
    TREE *newnode = (TREE *)malloc(sizeof(TREE));

    //if no memory is allocated display the error
    if(newnode == NULL)
    {
        printf("Memory Allocation Failed\n");
        return root;
    }

    //copy the contents to the newnode
    strcpy(newnode->name,w);
    strcpy(newnode->MobNum,mob);
    newnode->left = NULL;
    newnode->right = NULL;

    //If its the first ever node
    //assign it as root
    if(root == NULL)
    {
        root = newnode;
        return root;
    }

    //pointers to find the correct position of the newnode
    TREE *curr = root, *parent = NULL;


    //While there are nodes in the binary search tree
    while(curr != NULL )
    {
        //Finds the position of the newnode in alphabetical order
        while(tolower(w[i])!=tolower(curr->name[i]))
        {
          parent = curr;

        if(tolower(w[i]) < tolower(curr->name[i]))
        {

            curr = curr->left;

            if(curr!= NULL && i>0 && tolower(curr->name[i-1]) != tolower(w[i-1]))
               i=0;
        }
        else if(tolower(w[i]) > tolower(curr->name[i]))
        {

            curr = curr->right;

            if(curr!= NULL && i>0 && tolower(curr->name[i-1]) != tolower(w[i-1]))
               i=0;
        }

         if(curr == NULL)
            break;
        }
        if(curr != NULL  )
         i++;
    }


        if((int)tolower(w[i])<(int)tolower(parent->name[i]))
            parent->left = newnode;
        else
            parent->right = newnode;



    return root;

}



//Function:     Inorder
//Description:  Prints the Inorder traversal of the binary search tree
//Input param:  root node
//Return Type:  NULL
void Inorder(TREE *root)
{
    if(root != NULL)
    {
       Inorder(root->left);
       printf("%s %s\n",root->name,root->MobNum);
       Inorder(root->right);
    }

}




//Function:     sortNames
//Description:  Sorts and prints the name and mobile number from accounts table
//              in alphabetical order
//Input param:  NULL
//Return Type:  NULL
void sortNames()
{
    TREE *root = NULL;
    char w[30];
    char mob[25];

    //Insert name and mobile of every account into binary search tree
    for(int i=0;i<user_count;i++)
     {
       strcpy(w,account[i][1]);
       strcpy(mob,account[i][2]);

       root = insert_into_bst(root,w,mob);
    }

   //print the sorted users list
   printf("\n-------------------------------\n");
   printf("\t USERS' LIST");
   printf("\n-------------------------------\n");
   Inorder(root);

   write_log("sortNames", "SUCCESS", "Contents of accounts are sorted in alphabetical order based on the names\n");
}


//Function:     displayBalance
//Description:  Displays the balance amount of any particular account
//Input param:  NULL
//Return Type:  NULL
void displayBalance()
{
     char Googlepin[10];
     char mobno[20];
     char upi[10];
     int index;

     printf("\n--------------------\n");
    printf("Enter your Phone Number");
    printf("\n\nNote: In the actual Google Pay Application, \nMobile Number will be saved when account gets created the first time");
    printf("\n--------------------\n");
    scanf("%s",mobno);


   //Search for the mobile number from the accounts table
    for(int i=0 ; i < user_count ; i++)
    {
        for(int j=0; j< key_count; j++)
        {
            if(strcmp(account[i][j],mobno) == 0)
            {
                index = i;
                break;
            }
        }
    }

    //If mobile number is not found, it means that the number is not registered with google pay
    if(index == -1)
    {
        printf("Mobile number not found\n");
        printf("Enter 1 to Create account on Google Pay\n");
        write_log("displayBalance", "FAILURE", "Number not registered with Google Pay");
        return;
    }

    printf("\n--------------------\n");
    printf("Enter your Google Pin");
    printf("\n--------------------\n");

    scanf("%s",Googlepin);


    //If the entered is not present in the accounts table
    //stop the transaction and indicate the failure.
    if(index == -1)
    {
        printf("Invalid Mobile Number\n\n");
        write_log("displayBalance", "FAILURE", "Invalid Mobile Number");
        return;
    }


    //If the entered Pin does not match with the registered
    //Google Pin, stop the transaction and indicate the failure.
    if(atoi(account[index][0]) != atoi(Googlepin))
    {
        printf("Wrong Pin\n");
        write_log("displayBalance", "FAILURE", "Wrong Pin");
        return;
    }



    printf("\n----------------------\n");
    printf("Enter the UPI Pin");
    printf("\n----------------------\n");

    scanf("%s",upi);


    //If the enteredUPI Pin does not match
    //with the UPI Pin registered in accounts table, print as invalid
    if(atoi(account[index][4]) != atoi(upi))
    {
        printf("Invalid UPI Pin\n");
        write_log("displayBalance", "FAILURE", "Invalid UPI Pin");
        return;
    }

    printf("\n--------------------\n");
    printf("Balance = %s",account[index][5]);
    printf("\n--------------------\n");

    write_log("displayBalance", "SUCCESS", "Balance amount of the user displayed");

}
