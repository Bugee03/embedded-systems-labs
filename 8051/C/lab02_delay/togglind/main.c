#include <mcs51/8051.h>



void delay(unsigned int t)
{
    unsigned int i, j;
    for (i = 0; i < t; i++)
        for (j = 0; j < 120; j++);
}



void main(void) {

    while (1) {
        P1_0 = 0;
        delay(3000);
        P1_0 = 1;
        delay(3000);

        P1_1 = 1;
        delay(3000);
        P1_1 = 0;
        delay(3000);

        P1_2 = 1;
        P1_3 = 1;
        P1_4 = 1;
        P1_5 = 1;
        delay(2000); 
        P1_6 = 1;
    }
}

