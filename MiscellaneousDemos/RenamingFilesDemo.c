#include<stdio.h>

void main(){
     char th[] = "P:\\EazyFinder\\CVersion\\Version_1.1\\TransactionHistories\\temp1.txt";
     char newTHFileName[] = "P:\\EazyFinder\\CVersion\\Version_1.1\\TransactionHistories\\temp.txt";

     rename(th, newTHFileName);
}
