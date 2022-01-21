#include<stdio.h>
#include<conio.h>
#include<string.h>

void main()
{
     const int maxPasswordLength = 16;
     char password[maxPasswordLength + 1]; // 1 more space for '\0'
     char ch;
     int i = 0;
     printf("Password: ");
     
     while(1){
          ch = getch();
          if(ch == 13){ // if enter key
               if(i == 0){ // if password is empty
                    printf("\nPassword Cannot be Empty\n");
                    printf("Password: ");
               } else { // id password isn't empty, stop
                    break;
               }
          } else if(ch == 32 || ch == 9){ // if space or tab key is pressed ignore it
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

     printf("\n%s", password);
}