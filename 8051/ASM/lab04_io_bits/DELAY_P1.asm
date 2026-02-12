          ORG     0000H

BACK:     MOV     A,#55H
          MOV     P1,A
          LCALL   DELAY   ;waiting to new data
          MOV     A,#0AAH
          MOV     P1,A
          LCALL   DELAY
          SJMP    BACK

;------------this is the delay subroutine:
DELAY:    MOV     R5,#0FFH
AGAIN:    DJNZ    R5,AGAIN
          RET
          END
