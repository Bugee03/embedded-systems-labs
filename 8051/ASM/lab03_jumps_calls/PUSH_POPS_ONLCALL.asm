        ORG     0000H

BACK:   MOV     A,#055H       ; Load A with 55H
        MOV     P1,A          ; Output 55H to Port1

        MOV     R4,#099H      ; R4 preload
        MOV     R5,#067H      ; R5 preload
        LCALL   DELAY         ; Delay subroutine

        MOV     A,#0AAH       ; Load A with AAH
        MOV     P1,A          ; Output AAH to Port1
        LCALL   DELAY         ; Delay subroutine

        SJMP    BACK          ; Repeat forever


; -------------------------  
; DELAY SUBROUTINE
; -------------------------
        ORG     0300H

DELAY:  PUSH    4             ; Save R4
        PUSH    5             ; Save R5

        MOV     R4,#0FFH      ; Outer loop counter
NEXT:   MOV     R5,#0FFH      ; Inner loop counter
AGAIN:  DJNZ    R5,AGAIN      ; Decrement R5 until 0
        DJNZ    R4,NEXT       ; Then decrement R4

        POP     5             ; Restore R5
        POP     4             ; Restore R4
        RET                   ; Return to main program

        END
