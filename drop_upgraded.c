/************************************************************************************
*
*
*	Program to display a list of words which has the entered text dynamically
*
*
*************************************************************************************/
 #include<stdio.h>
 #include<string.h>
 #include<stdlib.h>


 #define NULL_CHAR 	'\0'			//Macro to specify null character
 #define SPACE		' '			//Macro to specify space
 #define NEXT_LINE	'\n'			//Macro to specify next line character
 
 #define READ 		"r"			//Macro for read option
 #define WRITE		"w"			//Macro for write option
 #define FILE_NAME	"dictionary1"	//Macro for file name
 
 #define BUFF_SIZE	50			//Max character size
 
 #define YES		1			
 #define NO 		0
 
 #define INITIALISE	0
 
 #define NO_MATCH		0
 #define MATCH		1
 #define PARTIAL_MATCH	2
 
 #define BACKSPACE	127			//ASCII value of Backspace character
 #define RETURN		13			//ASCII value of Return character
 
 #define NULL_STRING    0
 
 #define DIRECTION	27			//initial ASCII value of direction keys
 #define NO_DIR		0			//No direction flag
 #define DIR_UP		1			//Direction UP flag
 #define DIR_DOWN		2			//Direction DOWN flag
 #define DIR_RIGHT	3			//Direction RIGHT flag
 #define DIR_LEFT		4			//Direction LEFT flag
 
 #define START_PAGE	0			
 #define PAGE_SIZE	38			//Maximum number of words a page can hold
 
unsigned int stringlen(char *);
 /*
Function to determine the filesize of the given file
Arguments : File name
Returns   : File length/size
 */
unsigned long int filesize(char *fn)
{
	unsigned long int len;
	FILE *fp=fopen(fn,READ);
	if(fp==NULL)
	{
		printf("\nMentioned file is invalid !");
		exit(0);
	}
	fseek(fp,0,SEEK_END);								//Place file pointer at the last position
	len = (unsigned long int )ftell(fp);				 // Tell the position of the file pointer relative to the file
	rewind(fp);
	fclose(fp);
	return len;
}


 /*
 Function which copies the entire contents of the file to a character buffer
 Arguments : File name
 Returns   : Array of characters containing file contents
 */
 char* copy_buffer(char *fn)
{
	int len=filesize(fn);
	char *buffer=(char *)malloc(len*sizeof(char)+1); 			// Allocating memory for buffer including the null character
	if(buffer==NULL)
	{
		printf("Unable to allocate memory\n");
		return NULL;
	}
	FILE *fp=fopen(fn,READ);
	if(fp==NULL)
	{
		printf("\nMentioned file is invalid !\n");
		return NULL;
	}
	fread(buffer,sizeof(char)*len,1,fp);							//Read contents of file and store in buffer
	buffer[len]=NULL_CHAR;									//Append null character to terminate string
	rewind(fp);
	fclose(fp);
	return buffer;
}



/* 
Function to print the contents which matches the test string
Arguments : Array containing file contents , String to searched
Returns   : Nothing
*/
void list_contents(char *total,char *string,int direction)
{
	static int start,end;
	unsigned int count=INITIALISE;
	int flag=NO_MATCH;
	printf("\n");
	switch(direction)
	{
		case NO_DIR :{							
					start=START_PAGE;				//Initial page condition
					end=PAGE_SIZE;
				 }
				 break;
		case DIR_UP :{
					start-=PAGE_SIZE;				//Page UP condition
					end-=PAGE_SIZE;
					if(start<START_PAGE) 
					{
						start=START_PAGE;
						end=PAGE_SIZE;
					}
				}
				break;
		case DIR_DOWN :{
					start+=PAGE_SIZE;				//Page Down Condition
					end+=PAGE_SIZE;
				   }
				   break;
		default: break;
	}
		
	while(*total!=NULL_CHAR)
	{
		
		int i=INITIALISE;
		while(*total==*(string+i))						//Loop if letters are similar until finding mismatch
		{
			flag=PARTIAL_MATCH;
			total++;i++;
			if(*(string+i)==NULL_CHAR)				//Success criterion . Match Found.. Increment count and break
			{
				flag=MATCH;
				count++;
				total--;
				break;
			}
		}
		if(flag==MATCH && count>start && count<end)		//If match is obtained , print the corresponding word
		{
			total=total-stringlen(string)+1;			//Moving to the initial position of the word
			printf("\t\t\t\t\t\t\t\t\b\b");			// Tabbing for alignment
			while(*total!=NULL_CHAR && *total!=SPACE && *total!=NEXT_LINE)// Terminate loop when any of these are encountered
			{
				printf("%c",*total);
				total++;
			}
			printf("\n");
			if(*total!=NULL_CHAR)total++;
			flag=NO_MATCH;
		}
		else									//If no match is obtained , skip the word
		{
			while(*total!=NULL_CHAR && *total!=SPACE && *total!=NEXT_LINE) total++;
			if(*total!=NULL_CHAR)total++;
		}
		if(count>end)							//Print only contents which the current page can hold
		return;
	}
	if(count==0)
	{
		printf("No matches found");
	}
		
	if(count<start)								//If there are lesser matches than start , reset previous page
	{
		printf("Insufficient matches");
		start-=PAGE_SIZE;
		end-=PAGE_SIZE;
	}
	for(int i=0;i<end+1-count;i++)					//Fill next lines for situations with lesser contents
	printf("\n");
}

/*
Function to return the length of a given string
Arguments : Input String
Returns   : String length
*/
unsigned int stringlen(char* string)
{
	unsigned int count=INITIALISE;
	while(*string!=NULL_CHAR) 								// Loop until null character is encountered
	{ string++;count++;}
	return count;
}



int main(void)
{
 	int contin=YES,flag=NO_DIR;
 	char c=BACKSPACE,dummy;
	int currentpos=INITIALISE;
 	char *file=copy_buffer(FILE_NAME);						//Store contents of file in array
	if(file==NULL)
	{
		return 0;
	}
 	char *string=(char *)malloc(sizeof(char)*BUFF_SIZE);			//Buffer for input string
 	*string='\0';
	if(string==NULL)
	{
		printf("Unable to allocate memory");
		return 0;
	}
	while(contin==YES)
	{
		if(system("/bin/stty cooked")==-1) 					// Cooked input mode ..
		{	
			printf("Cannot execute system command");
			return 0;
		}
		printf("Enter the string to be searched ... Press Enter to quit , Up/Down to scroll through the list..\n%s_\n",string);
		list_contents(file,string,flag);					//List matching contents
		flag=NO_DIR;
		if(system("/bin/stty raw")==-1) 					// Raw input mode for getting characters
		{	
			printf("Cannot execute system command");
			return 0;
		}
		c=getchar();								//Get the next character from stdin
		if(c!=BACKSPACE && c!=RETURN && c!=DIRECTION)			//Append the character to string if not backspace or enter or dirs
		{	
			*(string+currentpos)=c;
			*(string+currentpos+1)=NULL_CHAR;				//Append null character
			currentpos++;
			//printf("\bNot backspace");
		}
		else if(c==BACKSPACE)
		{
			if(currentpos!=NULL_STRING)
			{
				currentpos--;
				*(string+currentpos)=NULL_CHAR;
			}
			else printf("No character entered\n");
			//printf("\bBackspace");
		}
		else if(c==DIRECTION)
		{
			if((c=getchar())==91)
			{
				switch((c=getchar()))
				{
					case 65 :	{
								printf("UP\n");
								flag=DIR_UP;
								break;
							}
					case 66 : 	{
								printf("DOWN\n");
								flag=DIR_DOWN;
								break;
							}
					case 67 : 	{
								printf("RIGHT\n");
								flag=DIR_RIGHT;
								break;
							}
					case 68 : 	{
								printf("LEFT\n");
								flag=DIR_LEFT;
								break;
							}
					default : break;
				}
			}
		}
		else
		{
			contin=NO;
		}
		system("clear");
	}
	system("clear");
	free(string);
	free(file);
	if(system("/bin/stty cooked")==-1) 
	{	
			printf("Cannot execute system command");
			return 0;
	}
 return 0;
 }
