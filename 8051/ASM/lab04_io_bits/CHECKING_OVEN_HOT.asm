;=====================================
; ex for oven,when oven is hot buzzer 
; start sounding to user than oven is
; ready for sth
;=====================================

ORG 0000H

HERE:   JNB P2.3,HERE     ;keep monitoring for high
        SETB P1.5         
        CLR  P1.5         ;make high-to low
        SJMP HERE         ;keep repeating


