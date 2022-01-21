#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<windows.h>
#include<conio.h>
#define maxPasswordLength 16

char username[20], password[maxPasswordLength+1]; // 1 is for string null character

long long encryptPassword(char* password){
     long long int code = 0, a = 1;
     int i, len = strlen(password);
     for(i = 0 ; i < len ; i++){
          code += ((int)password[i]) * a;
          a *= 100;
     }
     return code;
}

bool isPasswordAccepted(char* password, char ch){
     int len = strlen(password);
     bool passwordAccepted;
     if(len < 8){
          printf("\nPassword is too Short. Use atleast 8 Characters");
          passwordAccepted = false;
     } else if(len > maxPasswordLength) {
          printf("\nPassword is too Long. Reduce it to a range of 8 to %d Characters", maxPasswordLength);
          passwordAccepted = false;
     }
     int i;
     bool lowerCaseUsed = false, upperCaseUsed = false, splCharUsed = false, numUsed = false;
     for(i = 0 ; i < len ; i++){
          if(isalpha(password[i])){
               if(islower(password[i])) lowerCaseUsed = true;
               else upperCaseUsed = true;
          }
          else if(isalnum(password[i])) numUsed = true;
          else splCharUsed = true;
     }

     if(!lowerCaseUsed) printf("\nAtleast One Lower Case Character must be Used");
     if(!upperCaseUsed) printf("\nAtleast One Upper Case Character must be Used");
     if(!numUsed) printf("\nAtleast One Number must be Used");
     if(!splCharUsed) printf("\nAtleast One Special Character must be Used");

     if(passwordAccepted && lowerCaseUsed && upperCaseUsed && numUsed && splCharUsed)
          passwordAccepted = true;
     if(!passwordAccepted){
          if(ch != 'P')
               printf("\nPassword [Max of %d Characters]: ", maxPasswordLength);
          else {
               printf("\nEnter New Password: ");
          }
     }
     return passwordAccepted;
}

void passwordInput(char* password, char loginSignupCharacter, char passwordChangeCharacter){
     char ch;
     int i;
     do{
          i = 0;
          strcpy(password, "");
          while(true){
               ch = getch(); // getch() reads a key from keyboard without displaying it on screen and returns its ASCCI value
               if(ch == 13){ // if enter key
                    if(i == 0) printf("\nPassword Cannot be Empty");
                    break;
               } else if(ch == 32 || ch == 9){ // if space or tab key
                    continue;
               } else if(ch == 8){ // if backspace
                    if(i > 0){
                         i--;
                         password[i] = '\0';
                         printf("\b \b");
                    }
               } else{
                    if(i < maxPasswordLength){
                         password[i] = ch;
                         i++;
                         printf("*");
                    }
               }
          }
          password[i] = '\0';
          if(loginSignupCharacter == 'L') break;
     }while(!isPasswordAccepted(password, passwordChangeCharacter));
}

// SignUp and Login Code
bool SignUp_LogIn(char ch){
     char scannedUsername[20];
     long long int scannedPassword;
     bool found = false;
     FILE *adminFile;
     int i = 0;

     if(ch == 'S'){
          do{
               printf("Username: ");
               scanf("%s", username);
               adminFile = fopen("LogInSignUpDatabase.txt", "r");
               // Checking if Username is already present
               while(fscanf(adminFile, "%s %lld\n", scannedUsername, &scannedPassword) != EOF){
                    if(strcmp(scannedUsername, username) == 0){
                         found = true;
                         printf("Username already taken, Try with Another one\n");
                         break;
                    } else {
                         found = false;
                    }
               }
               fclose(adminFile);
          } while(found); // Only comes out when the username is not used by anyone

          printf("Set Password [Max of %d Characters]: ", maxPasswordLength);
          passwordInput(password, 'S', 'A');
          printf("\n");

          adminFile = fopen("LogInSignUpDatabase.txt" , "a");
          fprintf(adminFile, "%s %lld\n", username, encryptPassword(password));
          fclose(adminFile);
          printf("Account Created Successfully\n");
          
          char path[100], userFileName[100];
          getcwd(path, sizeof(path));
          strcat(path, "\\TransactionHistories\\");
          strcpy(userFileName, username);
          strcat(userFileName, ".txt");
          
          // Creation of file for user with username
          FILE *userFile = fopen(strcat(path, userFileName), "w");
          fclose(userFile);
          return true;
     }

     else{
          printf("Username: ");
          scanf("%s", username);

          printf("Password: ");
          passwordInput(password, 'L', 'A');
          printf("\n");

          adminFile = fopen("LogInSignUpDatabase.txt", "r");
          while(fscanf(adminFile, "%s %lld\n", scannedUsername, &scannedPassword) != EOF){
               if((strcmp(scannedUsername, username)) == 0 && scannedPassword == encryptPassword(password)){
                    printf("Logged In Successfully\n");
                    fclose(adminFile); // Closing file
                    return true;
               }
          }
          fclose(adminFile);
          printf("No User with the Given Credentials\n");
          return false;
     }
}

char callSignupLogin(){
     char SLChoice[7], ret;
     printf("SignUp or LogIn: ");
     scanf("%s",SLChoice);
     if(strcmp(strlwr(SLChoice), "signup") == 0)
          return SignUp_LogIn('S');
     return SignUp_LogIn('L');
}

void main()
{
     char returned, choice;
     do{
          returned = callSignupLogin();
          if(returned){
               // Write your Code Here
               printf("Welcome User!"); // Dummy Sentence
          } else {
               printf("Want to Try Again [Y/N]? ");
               scanf(" %c", &choice); // If 'Y' the user will get a chance to again signup/login
          }
          if(choice == 'N' || choice == 'n')
               printf("Have A Great Day Ahead! :)\n");
     }while(choice == 'Y' || choice == 'y');
}