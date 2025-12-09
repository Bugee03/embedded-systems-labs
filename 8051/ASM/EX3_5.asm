ORG 0000H

        CLR A
        MOV R5,A
        ADD A,#79H
        JNC N_1
        INC R5
N_1:    MOV A,#0F5H
        JNC N_2
        INC R5
N_2:    ADD A,#0E2H
        JNC OVER
        INC R5
OVER:   MOV R0,A

