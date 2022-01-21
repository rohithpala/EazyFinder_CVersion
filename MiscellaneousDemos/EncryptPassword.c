#include<stdio.h>
#include<string.h>

long long encryptPassword(char* password){
     long long int code = 0, a = 1;
     int i, len = strlen(password);
     for(i = 0 ; i < len ; i++){
          code += ((int)password[i]) * a;
          a *= 10;
     }
     return code;
}

void main(){
     char password[17];
     scanf("%s", password);
     printf("%lld", encryptPassword(password));
}