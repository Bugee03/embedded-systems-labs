ORG 0000H


SETB P1.2    ;receiver 
MOV A,#45    

AGAIN: JNB P1.2,AGAIN  ;out the loop when bit is high 
       MOV P0,A        ;data moved to P0 pin like sender 
       CLR P1.3       ;impulsing high bit and low like notice 
       SETB P1.3  
