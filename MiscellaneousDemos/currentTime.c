#include<stdio.h>
#include<time.h>

void main()
{
    time_t current_time;
    struct tm * time_info;
    char timeString[9]; // size is 9 because HH:MM:SS is 8 characters long and 1 more for '\0'

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
    printf("%s", timeString);
}