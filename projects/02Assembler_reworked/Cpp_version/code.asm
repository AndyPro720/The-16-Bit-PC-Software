//Check if a key is pressed then do the following :
//If pressed then from @SCREEN to the next 100 registers :
//                              Store -1
//Else                          Store 0 


(LOOP)
@SCREEN
D=A
@add           //address pointer
M=D

(CHECK)       //Checks if key pressed
@KBD
D=M
@KEY
D;JNE
@NOKEY
0;JMP

(KEY)        //while key pressed, store -1 in next 100 registers 
@16484     
D=A
@add
A=M
M=-1
@add
M=M+1
D=D-M
@KEY
D;JGE
@LOOP
0;JMP

(NOKEY)     //while no key pressed, store 0 in next 100 registers
@16484     
D=A
@add
A=M
M=0
@add
M=M+1
D=D-M
@hello
D;JGE
@LOOP
0;JMP