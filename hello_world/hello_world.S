.code64
.globl main
main:
push %RDI
mov	 $wtf, %RDI
call printf
POP %RDI
RET


.code32
.globl wtf
wtf:
DEC 	%EAX
INC 	%EBP
DEC 	%ESP
DEC 	%ESP
DEC 	%EDI
POP 	%EDI
PUSH 	%EDI
DEC 	%EDI
PUSH 	%EDX
DEC 	%ESP
INC 	%ESP
OR		(%EAX), %al
RET
