@256
D=A
@SP
M=D
//call sys.init0)
(Main.fibonacci)
@0
D=A
@ARG
A=M+D
D=M
@SP
A=M
M=D
@SP
M=M+1
@2
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
D=M-D
@TRUE.0
D;JLT
@SP
A=M-1
M=0
@NEXT.0
0;JMP
(TRUE.0)
@SP
A=M-1
M=-1
(NEXT.0)
@SP
AM=M-1
D=M
@IF_TRUE
D;JNE
@IF_FALSE
0;JMP
(IF_TRUE )
@0
D=A
@ARG
A=M+D
D=M
@SP
A=M
M=D
@SP
M=M+1
(IF_FALSE )
@0
D=A
@ARG
A=M+D
D=M
@SP
A=M
M=D
@SP
M=M+1
@2
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
M=M-D
@0
D=A
@ARG
A=M+D
D=M
@SP
A=M
M=D
@SP
M=M+1
@1
D=A
@SP
A=M
M=D
@SP
M=M+1
@SP
AM=M-1
D=M
A=A-1
M=M-D
@SP
AM=M-1
D=M
A=A-1
M=M+D
(Sys.init)
@4
D=A
@SP
A=M
M=D
@SP
M=M+1
(WHILE)
@WHILE 
0;JMP
(END)
@END
0;JMP