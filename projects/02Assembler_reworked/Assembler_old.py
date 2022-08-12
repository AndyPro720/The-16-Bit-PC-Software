# Loads the file up if in dir, else exits the program
#opens file to write upon

filename=input('Enter the name of the file(without extension) you wish to convert: ')
try: file_handle=open(filename+'.asm','r')
except: 
	print('No such file found') 
	exit()
file_handle_out=open(filename+'.hack','w')

#A instruction function
def a_ins(ins): 
		ins=str(bin(int(ins[1:])))
		file_handle_out.write(ins[2:].zfill(16)+'\n')  #start from 2 to avoid 0b id


# C instruction function
def c_ins(ins):
#parsing the induval sections out
	ndest=(ins.find('='))
	if ndest==-1: ndest=0 #as dest may be null
	ncomp=(ins.find(';'))
	if ncomp==-1: ncomp=len(ins) #as jump may be null
	
	dest=ins[:ndest]
	if ndest==0: ndest=-1  #when dest is null
	comp=ins[ndest+1:ncomp]
	jmp=ins[ncomp+1:len(ins)]
	
#converting the sections to binary
	destination = {
  "": "000",
  "M": "001",
  "D": "010",
  "MD": "011",
  "A": "100",
  "AM": "101",
  "AD": "110",
  "AMD": "111"
}

	jump = {
  "": "000",
  "JGT": "001",
  "JEQ": "010",
  "JGE": "011",
  "JLT": "100",
  "JNE": "101",
  "JLE": "110",
  "JMP": "111"
}

	compute = {
  "0": "0101010",
  "1": "0111111",
  "-1": "0111010",
  "D": "0001100",
  "A": "0110000",
  "M": "1110000",
  "!D": "0001101",
  "!A": "0110001",
  "!M": "1110001",
  "-D": "0001111",
  "-A": "0110011",
  "-M": "1110011",
  "D+1": "0011111",
  "A+1": "0110111",
  "M+1": "1110111",
  "D-1": "0001110",
  "A-1": "0110010",
  "M-1": "1110010",
  "D+A": "0000010",
  "D+M": "1000010",
  "D-A": "0010011",
  "D-M": "1010011",
  "A-D": "0000111",
  "M-D": "1000111",
  "D&A": "0000000",
  "D&M": "1000000",
  "D|A": "0010101",
  "D|M": "1010101"
}
	file_handle_out.write('111'+compute[comp]+destination[dest]+jump[jmp]+'\n')

#-----------------------------------------#
#Assembling starts here.
#strips the code of whitespace and comments

scode=''
for code in file_handle:
	
	code=''.join(code.split()) #remove whitespace
	if len(code)==0: continue
	else:
		num=code.find('//')   #comments
		if num==-1: num=len(code)
		code=code[0:num]
		if len(code)==0: continue
		else: scode=scode+code+'\n'
			

#converts symbols to their numeric counterparts
#symbols database
symbols = {
  "R0": "0",
  "R1": "1",
  "R2": "2",
  "R3": "3",
  "R4": "4",
  "R5": "5",
  "R6": "6",
  "R7": "7",
  "R8": "8",
  "R9": "9",
  "R10": "10",
  "R11": "11",
  "R12": "12",
  "R13": "13",
  "R14": "14",
  "R15": "15",
  "SCREEN": "16384",
  "KBD": "24576",
  "SP": "0",
  "LCL": "1",
  "ARG": "2",
  "THIS": "3",
  "THAT": "4",
}
#running first pass(fp) to look for (Labels) and stripping all () statements
fpcode=''
count=-1
for fpass in scode.splitlines():
	id=fpass.find('(')
	if id==-1: 
		count=count+1
		fpcode=fpcode+fpass+'\n'  
		continue
	else:
		id1=fpass.find(')')
		sym=fpass[id+1:id1]
		symbols[sym]=str(count+1)  #adding label and instruction number

#running second pass(sp) to look for var and storing them with register numbers
spcode=''
register=16
for spass in fpcode.splitlines():
	id=spass.find('@')
	if id==-1: 
		spcode=spcode+spass+'\n'
		continue
	else:
		#getting the variable name
		sym=spass[id+1:]
		try:
			#ignoring if already number
			int(sym)
			spcode=spcode+spass+'\n'
			continue
		except:
			try:
				#checking if already exists within the database
				sym=symbols[sym]
				spcode=spcode+'@'+sym+'\n'
			except:
				#else adding it
				symbols[sym]=str(register)
				register+=1
				sym=symbols[sym]
				spcode=spcode+'@'+sym+'\n'
        
        
#Sending second pass assembly code for trasnlation
for code in spcode.splitlines():
	if code[0]=='@': a_ins(code)
	else: c_ins(code)

print('File assembled with the same name')
file_handle_out.close()