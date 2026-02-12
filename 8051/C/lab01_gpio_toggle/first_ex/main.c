#include <8051.h>

void main(void){
  unsigned char mysymb[] = "012345ABCD";
  unsigned char z = 0;
  for(z = 0;z <= 255;z++){
    P1 = mysymb[z];
  }
}
