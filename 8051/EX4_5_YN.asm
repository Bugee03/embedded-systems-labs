;=========================================
; A switch is connected to pin1.7 write 
; the program to check the status of SW
; perform the following: 
; if SW=0 send letter "N" to P2
; if SW=1 send letter "Y"; to P2
;=========================================

ORG 0000H

SETB P1.7

AGAIN:    JB  P1.7,OVER
          MOV P2,#'N'
          SJMP AGAIN

OVER:     MOV P2,#'Y'
          SJMP AGAIN
