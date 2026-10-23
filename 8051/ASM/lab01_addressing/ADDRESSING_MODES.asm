;=============================================
; write a program to copy value 55H
; into RAM memory location 40H to 45H
; using:
; a) direct addressing mode
; b) register indirect add mode without loop
; c)with a loop 
ORG 0000H

;(a)
MOV A,#55H
MOV 40H,A
MOV 41H,A
MOV 42H,A
MOV 43H,A
MOV 44H,A

;(b)

MOV A,#33H
MOV R0,#50H
MOV @R0,A
INC R0
MOV @R0,A
INC R0
MOV @R0,A
INC R0
MOV @R0,A
INC R0
MOV @R0,A

;(c)

CLR A

MOV A,#22H
MOV R0,#60H
MOV R5,#5     ;COUNTER

LOOP:   MOV @R0,A
        INC R0
        DJNZ R5,LOOP
        CPL P1.2










