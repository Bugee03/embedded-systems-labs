ORG 0000H
;===========================
;   Carry flags example
;   File for EdSim51
;===========================


MOV A,#39H
ADD A,#2FH


;===========================
;even numb C =1,AC=1,P=0


CLR A
MOV A,#9CH
ADD A,#64H

;===========================
;   
; EXAMPLE 2-4 
;
;===========================

CLR A
MOV A,#88H
ADD A,#93H

HERE: SJMP HERE

END
