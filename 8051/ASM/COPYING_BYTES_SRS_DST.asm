ORG 0000H

MOV R0,#60H
MOV A,#10H
MOV R5,#16    ;COUNTER

LOOP:     MOV @R0,A
          INC R0
          DJNZ R5,LOOP
MOV A,#44H
MOV R1,#20H
MOV @R1,A
MOV R0,#70H
MOV R4,#10H

BACK:     MOV A,@R1
          MOV @R0,A
          INC R1
          INC R0
          DJNZ R4,BACK

END
