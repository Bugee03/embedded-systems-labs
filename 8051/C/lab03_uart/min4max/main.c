#include <8051.h>


void main(void){
	
	unsigned char numbs[] = {-4,-3,-2,-1,0,1,2,3,4};
	unsigned char counter = 0;

	for(counter = 0;counter <=8;counter++){
		P1 = numbs[counter];
	}
}


