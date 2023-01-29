#include "googlepay.h"
#include "locals.h"

void menu()
{
    printf("\n----------------------------------\n");
    printf("----------------------------------\n");
    printf("          GOOGLE PAY\n");
    printf("----------------------------------\n");
    printf("----------------------------------\n");
    printf("0-exit\n");
    printf("1-Print Account Details\n");
    printf("2-Create a New Account on Google Pay\n");
    printf("3-Make Transaction\n");
    printf("4-Search for accounts\n");
    printf("5-Display Minimum Cost to visit various locations\n");
    printf("6-Sort the Accounts according to balance amount of the users in non-decreasing order\n");
    printf("7-Sort the Accounts according to balance amount of the users in non-increasing order\n");
    printf("8-Display Google Pay users in Lexicographic order\n");
    printf("9-Know your Balance Amount\n");
    printf("--------------------------------\n");
}


int main()
{
    status = load();

    if(status != 1){
        printf("Load Failed\n");
        return 0;
    }

    while(1)
    {
        menu();
        printf("Enter your choice\n");
        scanf("%d", &choice);

        switch(choice)
        {
            case 0: printf("Data dumped to file\n");
                    status = dump();
                    if(status != 1)
                        printf("Dump Failed\n. Please see the tempfile.txt\n");
                    return 0;


            case 1: print_formatted();
                    break;

            case 2: add_account();
                     break;
            case 3:  Transaction();
                    break;
            case 4: printf("Search for a user\n");
                    printf("[Name / Mobile Number]\n");
                    printf("Note: Entering a subset of Name or mobile number will give the result\n");
                    printf("Ex: Entering 'a' will display all the GPay accounts having a 'a' in their names\n\n");
                    fflush(stdin);
                    scanf("%s",text);
                    printf("\n-----------------------------\n");
                    printf("CONTACTS");
                    printf("\n-----------------------------\n");
                    if(isdigit(text[0])==0)
                        displayProfilesbyName(text);
                    else
                        displayProfilesbyMobNumber(text);
                    break;
            case 5: displayDistances();
                    break;
            case 6: sortIncreasing();
                    break;
            case 7: sortDecreasing();
                    break;
            case 8: sortNames();
                    break;
            case 9: displayBalance();
                    break;

        }

    }
    return 0;
}
