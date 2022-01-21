#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(void)
{
    int i;
    srand(time(0)); // srand ensures that we get different values at different calls to rand
    printf("RAND_MAX: %d\n", RAND_MAX); // 32767
    for(i = 0; i < 5; i++){
        printf("%f ", (float)rand()/RAND_MAX); // Random Numbers b/w 0 and 1
        printf("%d\n", rand()); // Random Numbers below RAND_MAX
    }
    return 0;
}
