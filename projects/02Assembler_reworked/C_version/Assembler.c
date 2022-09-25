#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

//declaring all global variables and functions

char file_name[50] = "", instructions[1000000], binary[1000000]; 
struct map 
{
   char key[1000][100];
   char value[1000][10]; 
   int index;   //points to next empty index
};

char * itos(int number) {   //converts int to string and returns value 
    static char string[10];  
    snprintf(string, sizeof(string), "%d", number);   
    return string;
}

struct map symbol_table = {
   .key = {
         "R0",
         "R1",
         "R2",
         "R3",
         "R4",
         "R5",
         "R6",
         "R7",
         "R8",
         "R9",
         "R10",
         "R11",
         "R12",
         "R13",
         "R14",
         "R15",
         "SCREEN",
         "KBD",
         "SP",
         "LCL",
         "ARG",
         "THIS",
         "THAT"
        }, 
   
  .value = {
         "0",
         "1",
         "2",
         "3",
         "4",
         "5",
         "6",
         "7",
         "8",
         "9",
         "10",
         "11",
         "12",
         "13",
         "14",
         "15",
         "16384",
         "24576",
         "0",
         "1",
         "2",
         "3",
         "4"
        }, 

   .index = 23
};

struct map dest = {

    .value = {
         "000",
         "001",
         "010",
         "011",
         "100",
         "101",
         "110",
         "111"
        },
    .key = {
         "",
         "M",
         "D",
         "MD",
         "A",
         "AM",
         "AD",
         "AMD"
        },
    .index = 8
};

struct map jmp = {

    .value = {
         "000",
         "001",
         "010",
         "011",
         "100",
         "101",
         "110",
         "111"
        },
    .key = {
         "",
         "JGT",
         "JEQ",
         "JGE",
         "JLT",
         "JNE",
         "JLE",
         "JMP"
        },
    .index = 8
};

struct map comp = {

    .value = {
         "0101010",
         "0111111",
         "0111010",
         "0001100",
         "0110000",
         "1110000",
         "0001101",
         "0110001",
         "1110001",
         "0001111",
         "0110011",
         "1110011",
         "0011111",
         "0110111",
         "1110111",
         "0001110",
         "0110010",
         "1110010",
         "0000010",
         "1000010",
         "0010011",
         "1010011",
         "0000111",
         "1000111",
         "0000000",
         "1000000",
         "0010101",
         "1010101"
        },
    .key = {
         "0",
         "1",
         "-1",
         "D",
         "A",
         "M",
         "!D",
         "!A",
         "!M",
         "-D",
         "-A",
         "-M",
         "D+1",
         "A+1",
         "M+1",
         "D-1",
         "A-1",
         "M-1",
         "D+A",
         "D+M",
         "D-A",
         "D-M",
         "A-D",
         "M-D",
         "D&A",
         "D&M",
         "D|A",
         "D|M"
        },
    .index = 28
};

int mapf(char *id, char *value, struct map *table) {    //searches for id in key and puts value in second argument(if not found puts NULL) 
                                                        //Third argument gives map look in; Returns 1 if found, else 0.
    for(int j = 0; j < table->index; j++) {
       if(!strcmp(table->key[j], id)) {     //as strcmp returns 0 on succesful comparison
            strcpy(value, table->value[j]);
            return 1;
       } 
    }

    *value = '\0';
    return 0;
}

void file_handler (int read) {
   FILE * fp;
   char line[1000], *p;
   
   if(read) {

   do   //Open the file
   {
    printf("Please Input file name, without extension. (Ctrl+C to exit)\n");
    fgets(file_name, sizeof(file_name), stdin);

    p = strrchr(file_name, '\n');  //remove the trailing newline
    *p = '\0';

    char input[55];
    strcpy(input, file_name);
    strcat(input, ".asm"); //appending .asm extension
    fp = fopen(input, "r"); 


    } while(fp == NULL);
   
   //read the file, and store in instructions
   while(fgets(line, sizeof(line), fp) != NULL) {
       strcat(instructions, line);
   }
    fclose(fp);
    printf("\nFile read successfully.");
    printf("\n**************************************");
  }
   
   else {

    strcat(file_name, ".hack"); //appending .asm extension
    fp = fopen(file_name, "w");
    
    //write to file
    while(fputs(binary, fp));
    
    fclose(fp);
    printf("\nOutput file assembled with same name successfully.");
    printf("\n**************************************");

   }

}

void cleaner(void) {  //removes whitespace and comments

    char *dp, *sp;      //destination and string pointer
    dp = sp = instructions;

    do {        //clear empty space and comments

        while(*dp == ' ' || *dp == '\r' || *dp == '\f' || *dp == '\v'|| *dp == '\t')  dp++;   //skip through whitespace characters

        while(*dp == '/' && *(dp+1) == '/') { while(*dp != '\n') dp++; }      //skip till next line if comments(//)

    }while(*sp++ = *dp++);      //puts the character pointed by(*dp) in the location pointed by string pointer(*sp), thus rewriting string.
                                // Increments both to next char, until dp returns NULL.
    
    dp = sp = instructions; //reset pointers
    do {        //clear any consecutive newlines(empty lines) 
        if(instructions[0] == '\n') dp++;   //if first line is empty 

        while(*dp == '\n' && *(dp-1) == '\n') { dp++; continue; }       
    }while(*sp++ = *dp++);
    
    
    printf("\nCleaner executed successfully.");
    printf("\n**************************************");
}

void first_pass_labels(void) {          //Extract label and add it to symbol table with value(line number)
    char *sp, *dp, label[100], *lp;
    sp = dp = instructions;
    lp = label;
    int line = 0;
    
    
    do {
        while(*dp == '(') {        //find label (while as labels may be consecutive)
            dp++; 
            while(*dp != ')') *lp++ = *dp++;  //store label refrence in label var 
            *lp = '\0';         
            
            strcpy(symbol_table.key[symbol_table.index], label);
            strcpy(symbol_table.value[symbol_table.index++], itos(line));
            
            dp += 2;   //skip ) and \n
            lp = label;  //reset label pointer
        }  

        if(*dp == '\n') line++;     //count lines

    }while(*sp++ = *dp++);     

    printf("\nFirst Pass executed successfully.");
    printf("\n**************************************");
    
}

void second_pass_var(void) {        //stores variables, replaces label and variable calls with value
    char *sp, *dp, *id, *v, identifer[100], value[100];
    sp = dp = instructions;
    int reg = 15;
    
    do {
    id = identifer, v = value; //reset pointers

        if(*dp == '@' && !(isdigit(*(dp+1)))) {    //if A instruction and not reg declaration
           *sp++ = *dp++; //write @ 

           while(*dp != '\n' && *dp != '\0') *id++ = *dp++;    //extract identifier and store, /0 condition for if last line 
           *id = '\0';

           if(mapf(identifer, value, &symbol_table)) {    //if id found, write value after @
             while(*v != '\0') *sp++ = *v++;
           }
           
           else {       //store variable and value, write after @ 
            strcpy(symbol_table.key[symbol_table.index], identifer);
            strcpy(symbol_table.value[symbol_table.index++], itos(++reg));
            
            strcpy(value, itos(reg));
            while(*v != '\0') *sp++ = *v++;     //write value
           }
       }    
    }while(*sp++ = *dp++);

    printf("\nSecond Pass executed successfully.");
    printf("\n**************************************");
    }  

void translator(void) {     //translate instructions to 16bit binary code

    char *sp, *dp, *lp, line[1000];
    sp = instructions; //string pointer
    dp = binary;    //destination pointer
    lp = line;      //line pointer

    do{     //read by line and send for translation 
       while(*sp != '\n' && *sp != '\0')  *lp++ = *sp++;    //read line and store it (\0 condition for last line)  
       *lp = '\0';  
        lp = line;   //reset line pointer 
       
      if(line[0] == '@') {  //if A instructions
          char bin[17], *bp = bin;

          long p = 16, i = strtol((line+1), NULL, 10);  //convert string to int/long
          
          while(p--) *bp++ = '0';   //add padding of 16 zeros 
          *bp = '\0';   

          do{       //converts to binary (gives bin in reverse, hence stored from end) 
            (*(--bp) = (i % 2) > 0 ? '1' : '0');
          }while(i/=2);
          bp = bin;

          while(*bp != '\0') *dp++ = *bp++;     //write to destination pointer 
          *dp++ = *sp;    //add line ending
      } 
      
      else {    //for C instructions
         //dest = comp; jmp 
          char c[6], j[4], id[6] = "", *idp = id; 
          char d[10];   //weird bug if declared above
          int d_flag = 1, j_flag = 1;

          if(strchr(line, '=')) {   //if dest exists, get d and disable flag
            while(*lp != '=') *idp++ = *lp++;
            idp = '\0', lp++;

            mapf(id, d, &dest); 
            idp = id, d_flag = 0;
          }

          if(strchr(line, ';')) {   //if jmp exists

            while(*lp != ';') *idp++ = *lp++;   //get comp
            idp = '\0', lp++;
            mapf(id, c, &comp); 
            idp = id;
            
            while(*lp != '\0') *idp++ = *lp++;    //get jmp
            idp = '\0';
            mapf(id, j, &jmp);
            idp = id, j_flag = 0;   //disable flag
          }
          
          if(*lp != '\0') {    //if comp not grabbed yet
              while(*lp != '\0') *idp++ = *lp++;
              idp = '\0';
              mapf(id, c, &comp); 
              idp = id;
          }
          
          //if dest/jmp don't exist
          if(d_flag) mapf("", d, &dest);
          if(j_flag) mapf("", j, &dest);
          
          char *cp = c, *d_p = d, *jp = j;
          
          //write bin (111 comp dest jmp)
          for(int x = 3; x > 0; x--) *dp++ = '1';
          while(*cp != '\0') *dp++ = *cp++; 
          while(*d_p != '\0') *dp++ = *d_p++; 
          while(*jp != '\0') *dp++ = *jp++; 
          *dp++ = *sp;  //add endline char
      } 
        lp = line;   //reset line pointer 

    }while(*sp++ != '\0');     //goes to next char(line) if line ending is not \0 
    
    printf("\nTranslation completed successfully.");
    printf("\n**************************************");
}

int main (void) {   //Take an assembly file as input and create an output file with the translated machine code

    file_handler(1);    //read
    cleaner();
    first_pass_labels();
    second_pass_var();
    translator();
    file_handler(0);    //write
    return 0;
}