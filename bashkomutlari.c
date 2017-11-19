#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 80 
#define BUFFER_SIZE 50
#define buffer "\n\Shell Command History:\n"

char history[BUFFER_SIZE][BUFFER_SIZE]; 
int count = 0; //The total number of commands entered holds.

int CommandControl(char inputBuffer[], char *args[],int *flag) 
{
     	int length, start, hist; 
     	int i, j=0,k, counter=0, counter2=0; 
     	length = read(STDIN_FILENO, inputBuffer, MAX_LINE); //read the commands
     	start = -1;
     
     if (length <= 1) 
     {
  	printf("Command not read\n");
  	exit(-1); 
     }
    
     for (i=0;i<length;i++)
     {  
	//Encountering a special character.
 	switch (inputBuffer[i])
  	{
      	case ' ':
      	case '\t' :      
          if(start != -1)
          {
              args[j] = &inputBuffer[start];  
              j++;
          }
          inputBuffer[i] = '\0'; 
          start = -1; 
          break;
          
      	case '\n': //final char
          if (start != -1)
          {
              args[j] = &inputBuffer[start];
              j++;
          }
          inputBuffer[i] = '\0';
          args[j] = NULL; //no more arguments
          break;
      	case '&':
       	      *flag  = 1; //concurrent.
              inputBuffer[i] = '\0';
                 break;
      	default :           
          if (start == -1)
              start = i; //index determination
   break;
  }
     }
     args[j] = NULL; //for program execution	

 
if(strcmp(args[0],"history")==0) 
{
	int index[count]; 
	int temp;
	char command[count][count];
	int command2 = 0;
	int inc=0;
   	if(count>0) //if there is a command
   	{
   		for(i = 0; i<count; i++)
   		{
           		counter = 1;
			counter2 =1;
			if(i>0)
			{
				for(k = i-1; k>=0 ; k--) //Compared to previous commands itself.
			    	{
     					if(strcmp(history[i],history[k]) == 0) 
            				{
      						counter2 = 0;
      						break;
            				}
     					else
      						counter2 = 1;
         
    				}
    			}
    			if(counter2 == 1)
    			{
    				for(j = i+1; j<count; j++) //Compared to next commands itself.
           			{ 
     					if(strcmp(history[i],history[j]) == 0)
     					{
         					counter++;
     					}
    				}
    				index[inc] = counter; //The number of entered commands holds.
    				strcpy(command[inc],history[i]); //Entered different commands holds.
    				command2++;
    				inc++;
    			}
   	}	
   char temp2[command2][command2]; 
   //Sorting
   for(i = 1; i<command2; i++)
   {
    	temp = index[i]; 
    	strcpy(temp2[i],command[i]);
    	j = i;
    	while(j>0 && index[j-1] < temp)
    	{
     		index[j] = index[j-1];
     		strcpy(command[j],command[j-1]);
     		j--;

    	}
    	index[j] = temp;
    	strcpy(command[j],temp2[i]);
   }
   //Display the sorted commands
   for(i = 0; i<inc; i++)
   {
	if(i != 10)
    		printf("%d.Command : %s --> %d \n",i+1, command[i], index[i]);
	else
		break;
   }
   }
   else
   {
   	printf("\nNo Commands in the history\n");
   }
   return -1;
  }
  //When enter the 'exit' command
  else if(strcmp(args[0],"exit")==0)
  {
  	exit(-1);
  }
     strcpy(history[count], inputBuffer);
     count++; 
 }
int main(void)
{
    char inputBuffer[MAX_LINE]; //the sequence holding commands
    int flag; //there is & or no
    char *args[MAX_LINE/2 + 1]; //command line arguments
    int should_run =1; 
    
    pid_t pid;
   
    while (should_run)
    {            
        flag = 0; //default flag value
        printf("osh>");
        fflush(stdout); //remove the output
        if(CommandControl(inputBuffer,args,&flag) != -1) //get next command
 	{
  	pid = fork();
        
         if (pid < 0)
         {
              printf("Fork failed.\n");
              exit (1);
         }
        
          else if (pid == 0)
         {      
	      //child process
              if (execvp(args[0], args) == -1)
              {
                  printf("Error executing command\n");
              }
          }
         else
         {
              //parent process
              if (flag == 0)
              {
                  //there is no & at the end of command
		  //Parent process waits for child process.
		  printf("Parent process waits for child process..\n");
                  wait(NULL);
              }
   
   	      if(flag == 1)
   	      {
		 //parent and child processes work concurrent.
		 printf("Parent and child processes work concurrent..\n");
       		exit(1);    
   	      }
         }
     	}
   }
return 0;
}