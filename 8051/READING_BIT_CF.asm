ORG 000H

SETB P1.7
;CLR P1.2
AGAIN:      MOV C,P1.2
            JC  OVER
            MOV P2,#'N'
            SJMP AGAIN

OVER:       MOV P2,#'Y'
            SJMP AGAIN
