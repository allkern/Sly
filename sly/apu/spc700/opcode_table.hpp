NOP 1, 2
TCALL, IMP, 8
SET1 d.0 2, 4
BBS, BRL, 5
ORA, d 2, 3
ORA, !a 3, 4
ORA, (X) 1, 3
ORA, [d+X] 2, 6
ORA, #1 2, 2
OR dd, ds 3, 6
OR1 C, m.b 3, 5
ASL d 2, 4
ASL la 3, 5
PUSH PSW 1, 4
TSET1 la 3, 6
BRK 1, 8

BPL r 2, 2
TCALL, IMP, 8
CLR1 d.0 2, 4
BBC, BRL, 5
ORA, d+X 2, 4
ORA, !a+X 3, 5
ORA, !a+Y 3, 5
ORA, [d]+Y 2, 6
OR d, #1 3, 5
OR (X), (Y) 1, 5
DECW d 2, 6
ASL d+X 2, 5
ASL A 1, 2
DEC X 1, 2
CMP X, la 3, 4
JMP [!a+X] 3, 6

CLRP 1, 2
TCALL, IMP 1, 8
SET1 d.1 2, 4
BBS, BRL, 5
AND A, d 2, 3
AND A, !a 3, 4
AND A, (X) 1, 3
AND A, [d+X] 2, 6
AND A, #i 2, 2
AND dd, ds 3, 6
OR1 C, im.b 3, 5
ROL d 2, 4
ROL la 3, 5
PUSH A 1, 4
CBNE d, r 3, 5
BRA r 2, 4

BMI r 2, 2
TCALL, IMP 1, 8
CLR1 d.1 2, 4
BBC, BRL, 5
AND A, d+X 2, 4
AND A, !a+X 3, 5
AND A, !a+Y 3, 5
AND A, [d]+Y 2, 6
AND d, #1 3, 5
AND (X), (Y) 1, 5
INCW d 2, 6
ROL d+X 2, 5
ROL A 1, 2
INC X 1, 2
CMP X, d 2, 3
CALL !a 3, 8

SETP 1, 2
TCALL, IMP 1, 8
SET1 d.2 2, 4
BBS, BRL, 5
EORA, d 2, 3
EORA, !a 3, 4
EORA, (X) 1, 3
EORA, [d+X] 2, 6
EORA, #i 2, 2
EOR dd, ds 3, 6
AND1 C, m.b 3, 4
LSR d 2, 4
LSR la 3, 5
PUSH X 1, 4
TCLR1 la 3, 6
PCALL u 2, 6

BVC r 2, 2
TCALL, IMP 1, 8
CLR1 d.2 2, 4
BBC, BRL, 5
EORA, d+X 2, 4
EORA, !a+X 3, 5
EORA, !a+Y 3, 5
EORA, [d]+Y 2, 6
EOR d, #1 3, 5
EOR (X), (Y) 1, 5
CMPW YA, d 2, 4
LSR d+X 2, 5
LSR A 1, 2
MOV X, A 1, 2
CMP Y, !a 3, 4
JMP !a 3, 3

CLRC 1,2
TCALL, IMP 1,8
SET1 d.3 2,4
BBS, BRL,5
CMP A, d 2,3
CMP A, !a 3,4
CMP A, (X) 1,3
CMP A, [d+X] 2,6
CMP A, #i 2,2
CMP dd, ds 3,6
AND1 C, im.b 3,4
ROR d 2,4
ROR la 3,5
PUSH Y 1,4
DBNZ d, r 3,5
RET 1, 5

BVS r 2, 2
TCALL, IMP 1, 8
CLR1 d.3 2, 4
BBC, BRL, 5
CMP A, d+X 2, 4
CMP A, !a+X 3, 5
CMP A, !a+Y 3, 5
CMP A, [d]+Y 2, 6
CMP d, #1 3, 5
CMP (X), (Y) 1, 5
ADDW YA, d 2, 5
ROR d+X 2, 5
RORA 1, 2
MOV A, X 1, 2
CMP Y, d 2, 3
RETI 1, 6

SETC 1, 2
TCALL, IMP 1, 8
SET1 d.4 2, 4
BBS, BRL, 5
ADC A, d 2, 3
ADC A, !a 3, 4
ADC A, (X) 1, 3
ADC A, [d+X] 2, 6
ADC A, #i 2, 2
ADC dd, ds 3, 6
EOR1 C, m.b 3, 5
DEC d 2, 4
DEC !a 3, 5
MOV Y, #i 2, 2
POP PSW 1, 4
MOV d, #i 3, 5

BCC r 2, 2
TCALL, IMP 1, 8
CLR1 d.4 2, 4
BBC, BRL, 5
ADC A, d+X 2, 4
ADC A, la+X 3, 5
ADC A, la+Y 3, 5
ADC A, [d]+Y 2, 6
ADC d, #1 3, 5
ADC (X), (Y) 1, 5
SUBW YA, d 2, 5
DEC d+X 2, 5
DEC A 1, 2
MOV X, SP 1, 2
DIV YA, X 1, 12
XCN A 1, 5

EI 1, 3
TCALL, IMP 1, 8
SET1 d.5 2, 4
BBS, BRL, 5
SBC A, d 2, 3
SBC A, !a 3, 4
SBC A, (X) 1, 3
SBC A, [d+X] 2, 6
SBC A, #i 2, 2
SBC dd, ds 3, 6
MOV1 C, m.b 3, 4
INC d 2, 4
INC la 3, 5
CMP Y, #1 2, 2
POP A 1, 4
MOV (X)+, A 1, 4

BCS r 2, 2
TCALL, IMP 1, 8
CLR1 d.5 2, 4
BBC, BRL, 5
SBC A, d+X 2, 4
SBC A, la+X 3, 5
SBC A, la+Y 3, 5
SBC A, [d]+Y 2, 6
SBC d, #1 3, 5
SBC (X), (Y) 1, 5
MOVW YA, d 2, 5
INC d+X 2, 5 
INC A 1, 2
MOV SP, X 1, 2
DAS A 1, 3
MOV A, (X)+ 1, 4

DI 1, 3
TCALL, IMP 1, 8
SET1 d.6 2, 4
BBS, BRL, 5
MOV d, A 2, 4
MOV la, A 3, 5
MOV (X), A 1, 4
MOV [d+X], A 2, 7
CMP X, #i 2, 2
MOV la, X 3, 5
MOV1 m.b, C 3, 6
MOV d, Y 2, 4
MOV la, Y 3, 5
MOV X, #i 2, 2
POP X 1, 4
MUL YA 1, 9

BNE r 2, 2
TCALL, IMP 1, 8
CLR1 d.6 2, 4
BBC, BRL, 5
MOV d+X, A 2, 5
MOV la+X, A 3, 6
MOV la+Y, A 3, 6
MOV [d]+Y, A 2, 7
MOV d, X 2, 4
MOV d+Y, X 2, 5
MOVW d, YA 2, 5
MOV d+X, Y 2, 5
DEC Y 1, 2
MOV A, Y 1, 2
CBNE d+X, r 3, 6
DAA A 1, 3

CLRV 1, 2
TCALL, IMP 1, 8
SET1 d.7 2, 4
BBS, BRL, 5
MOV A, d 2, 3
MOV A, !a 3, 4
MOV A, (X) 1, 3
MOV A, [d+X] 2, 6
MOV A, #i 2, 2
MOV X, la 3, 4
NOT1 m.b 3, 5
MOV Y, d 2, 3
MOV Y, la 3, 4
NOTC 1, 3
POP Y 1, 4
SLEEP 1, 3

BEQ r 2, 2
TCALL, IMP 1, 8
CLR1 d.7 2, 4
BBC, BRL, 5
MOV A, d+X 2, 4
MOV A, la+X 3, 5
MOV A, la+Y 3, 5
MOV A, [d]+Y 2, 6
MOV X, d 2, 3
MOV X, d+Y 2, 4
MOV dd, ds 3, 5
MOV Y, d+X 2, 4
INC Y 1, 2
MOV Y, A 1, 2
DBNZ Y, r 3, 5
STOP 1,3 