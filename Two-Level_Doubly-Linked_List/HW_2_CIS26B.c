/*
**  Date: 01/25/2016   
**  IDE (compiler): GCC
**  Memory Leak Detection: Valgrind 3.11.0
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h> //for tolower().

#define DUMMY_TRAILER '\177'
					 // octal ASCII code of the
					 // last character in the ASCII table

//Define NODES:
typedef struct computer_node COMPUTER_NODE;
struct computer_node
{
	 char computerName[22];
	 char computerStatus;
	 COMPUTER_NODE *forw;
	 COMPUTER_NODE *back;
};

typedef struct park_node PARK_NODE;
struct park_node
{
	 char parkName[32];
	 int duplicate;
	 COMPUTER_NODE *computerList;
	 PARK_NODE *forw;
	 PARK_NODE *back;
};

//Function prototypes:

void readFromFile (char inputFileName [], PARK_NODE *parkList);
void printParkList (PARK_NODE *parkList, int ascendingOrDescending, int isWithComputerList);
void getUserInputAndSearch(PARK_NODE *parkList);
char *searchInputComputer (PARK_NODE *parkList, char parkInput[], char computerInput[]);
char *searchNearestUpComputer (COMPUTER_NODE *pComputer);
void deallocNodes (PARK_NODE* parkList);

//PARK_NODE functions:
PARK_NODE *init_parkList(void);
PARK_NODE *insert_park(PARK_NODE *parkList, char *parkName);
void traverse_forw_park(PARK_NODE *parkList, int isWithComputerList);
void traverse_back_park(PARK_NODE *parkList, int isWithComputerList);

//COMPUTER_NODE functions:
COMPUTER_NODE *init_computerList(void);
int insert_computer(COMPUTER_NODE *computerList, char *computerName, char computerStatus);
void traverse_forw_computer(COMPUTER_NODE *computerList);
void traverse_back_computer(COMPUTER_NODE *computerList);

//String related functions:
char* strlwr(char* s);
int isAlphaOnly(char *s);


int main( void )
{
	//Limit file name to 100 characters.
	char inputFileName[100];

	//Ask for user input and output file name, default file name would be used 
	//if no input.
	printf("\nEnter name of input file : ");
	fgets(inputFileName, sizeof(inputFileName), stdin);
	if (inputFileName[0]=='\n')
	{
		strcpy(inputFileName,"comprings.txt");
	}  
	printf("Input file's name is: %s",inputFileName);

	PARK_NODE *parkList = init_parkList();
	readFromFile(inputFileName,parkList);

	printf("\n\nAfter readFromFile(),all park names "
			"without computer list in ascending order (A to Z):");
	printParkList (parkList, 1, 0);

	printf("\n\nAfter readFromFile(),all park names "
			"in descending order (Z to A) with computer list at each park:");
	printParkList (parkList, 0, 1);

   getUserInputAndSearch(parkList);

	//Free all the dynamic memory:
   deallocNodes(parkList);

	return 0;
}


//Beginning of function definitions:

//Building and populate linked lists based on input from file.
void readFromFile (char inputFileName [], PARK_NODE *parkList)
{
	FILE *fp = fopen(inputFileName, "r");
	if (fp == NULL)
	{
		printf ("Error opening file for reading.");
		exit(101);  
	}

	char line[256];
	char computerName[22];
	char computerStatus = '0';
	int duplicate = 1;

	PARK_NODE *currentPark;

	while(fgets(line, 256, fp)!=NULL) 
	{
		if(line[strlen(line) - 2]!= '#')
		{
			line[strlen(line)-1]='\0';//Remove the line feed '\n'.
			currentPark = insert_park(parkList, line);

			if(currentPark->duplicate)
			{
				printf("\"%s\" already in the list!\n",line);
			}
		}else
		{
			if(currentPark->duplicate)
			{
				continue;
			}

			char *temp = line;
			while (sscanf(temp, "%s %c", computerName, &computerStatus))
			{
				duplicate = insert_computer(currentPark->computerList, 
					computerName,computerStatus);
				if(duplicate)
				{
					printf("\"%s\" already in the list!\n",computerName);
				}

				if(*(temp+strlen(computerName)+2)!='#')
				{
					temp+=strlen(computerName)+3;
				}
				else
					break;
			}
		}
	}

	//Close input file,print error if there is any.
	if (fclose(fp) == EOF)
	{
		printf ("\nError closing file!\n");
		exit(201); 
	}
}

//Let client use int to indicate how & what to print.
void printParkList (PARK_NODE *parkList, int ascendingOrDescending, int isWithComputerList)
{ 
	if (ascendingOrDescending == 0)
		traverse_back_park(parkList,isWithComputerList);
	else
		traverse_forw_park(parkList,isWithComputerList);
}

//Read user input, search for the park and computer and print out findings.
void getUserInputAndSearch(PARK_NODE *parkList)
{
	char parkInput[32];

	char computerInput[22];
	char *computerReadIn;
	const char delimiterOfComputerInput[2] = " ";

	char next[5];

	int attemptCounterForParkName = 0;
	int attemptCounterForComputerName = 0;

	do{
		//Start handling input for park name:
		printf ("\n===============================\nEnter Park Name: ");
		fgets(parkInput, sizeof(parkInput), stdin);
		if(parkInput[0]!='\0')
			parkInput[strlen(parkInput)-1]='\0';//Remove the line feed '\n'.

		if(!isAlphaOnly(parkInput))
		{
			if (attemptCounterForParkName < 3)
			{
				printf("Invalid: Park Name should not include digits or any"
				 		 " non-letter character.Try again:\n");
				attemptCounterForParkName++;
				continue;				
			}

			printf("Too many failed attempts for park name, exiting the program.\n\n");
			exit(0);
		}

		//Start handling input for computer name:
		printf("Enter Computer Name: ");
		fgets(computerInput, sizeof(computerInput), stdin);

		if(computerInput[0]!='\0')
			computerInput[strlen(computerInput)-1]='\0';//Remove the line feed '\n'.

		computerReadIn = strtok(computerInput, delimiterOfComputerInput);

		if(!isAlphaOnly(computerReadIn))
		{
			if (attemptCounterForComputerName < 3)
			{
				printf("Invalid: Computer Name should not include digits or any"
						 " non-letter character.Start over:\n");
				attemptCounterForComputerName++;
				continue;				
			}
			printf("Too many failed attempts for computer name, "
					 "exiting the program.\n\n");
			exit(0);
		}

		printf("\nRead-in of Park Name: %s | length: %lu\n"
				"Read-in of Computer Name: %s | length: %lu\n\n"
				"Start Searching...\n\n",
				parkInput,strlen(parkInput),computerReadIn,strlen(computerReadIn));

		printf("%s\n===============================\n", 
				searchInputComputer(parkList, parkInput, computerReadIn));

		printf("Do you want to quit? If so, type quit, otherwise type any key:\n");
		fgets(next, sizeof(next), stdin);

	}while(strcmp(next,"quit") && strcmp(next,"QUIT"));

	printf("\nGood-bye!\n\n");

}

//Search for the computer user input.
char *searchInputComputer (PARK_NODE *parkList, char parkInput[], char computerInput[])
{
	/* 
	retString need to be static to avoid warning. need to reset to empty
	string at the beginning of each function call.
	*/
	static char retString[256];
	retString[0]='\0';
	strcat(retString, "Search Result:\n");

	do{
		parkList = parkList->forw;

		if(!strcmp(strlwr(parkList->parkName), strlwr(parkInput)))
		{
			//Found the park.
			COMPUTER_NODE *pComputerList = parkList->computerList;
			do{
					pComputerList = pComputerList->forw;

					if(!strcmp(strlwr(pComputerList->computerName), strlwr(computerInput)))
					{
						//Found the computer:
						strcat(retString, "Both Park and Computer are Found.\n");
						strcat(retString, searchNearestUpComputer(pComputerList));
						return retString;
					}	

			}while (pComputerList->computerName[0] != DUMMY_TRAILER);

			//Computer Not Found:
			strcat(retString,"Park Found but Computer Not Found.\n");
			return retString;
		}	

	}while (parkList->parkName[0] != DUMMY_TRAILER);

	//Park Not Found:
	strcat(retString,"Park Not Found.");
	return retString;

}

//Search for 'up' computer on the left side and right hand side.
char *searchNearestUpComputer (COMPUTER_NODE *pComputer)
{
	/* 
	retString need to be static to avoid warning. need to reset to empty
	string at the beginning of the function call.
	*/
	static char retString[256];
	retString[0]='\0';

	int upComputerFoundOnLeft=0;
	int upComputerFoundOnRight=0;

	COMPUTER_NODE *pSearchLeft = pComputer;
	COMPUTER_NODE *pSearchRight = pComputer;

		//Search to the left:	
		do{
			pSearchLeft = pSearchLeft->back;

			if (pSearchLeft->computerStatus=='1')
			{
				strcat(retString,"The nearest 'up' computer on the left is: ");
				strcat(retString,pSearchLeft->computerName);
				strcat(retString,", ");
				upComputerFoundOnLeft=1;
				break;
			}

		}while(pSearchLeft->computerName[0] != DUMMY_TRAILER);

		if (upComputerFoundOnLeft==0)
		{
			strcat(retString,"No 'up' computer on the left, ");
		}

		//Search to the right:
		do{
			pSearchRight = pSearchRight->forw;

			if (pSearchRight->computerStatus=='1')
			{
				strcat(retString,"the nearest 'up' computer on the right is: ");
				strcat(retString,pSearchRight->computerName);
				strcat(retString,".");
				upComputerFoundOnRight=1;
				break;
			}

		}while(pSearchRight->computerName[0] != DUMMY_TRAILER);

		if (upComputerFoundOnRight==0)
		{
			strcat(retString,"no 'up' computer on the right.");
		}

		return retString;
}

/***************************************************
	 Initialization of a circularly doubly-linked
	 list with one sentinel node
*/

PARK_NODE *init_parkList(void)
{
	 PARK_NODE *parkList;

	 // allocate the sentinel node
	 parkList = (PARK_NODE *) malloc(sizeof (PARK_NODE));
	 if (!parkList)
	 {
		  printf("Error in init_list!\n");
		  exit(1);
	 }

	 parkList->parkName[0] = DUMMY_TRAILER;
	 parkList->parkName[1] = '\0';
	 parkList->forw = parkList;
	 parkList->back = parkList;
	 parkList->computerList = NULL;

	 return parkList;
}

/***************************************************
	 Insert a node in a sorted circularly
	 doubly-linked list with one sentinel node
		  return 1 - if duplicate
		  return 0 - otherwise
*/
PARK_NODE *insert_park(PARK_NODE *parkList, char *parkName)
{
	 PARK_NODE *curr = parkList->forw;
	 PARK_NODE *prev = parkList;
	 PARK_NODE *pnew;

	 // search the right position to insert
	 while (strcmp(parkName,curr->parkName) > 0)
	 {
		  prev = curr;
		  curr = curr->forw;
	 }

	 pnew = (PARK_NODE *) malloc(sizeof (PARK_NODE));
	 if (!pnew)
	 {
		printf("Fatal memory allocation error in insert!\n");
		exit(3);
	 }

	 pnew->duplicate = 1;

	 if (strcmp(parkName, curr->parkName))
	 {
		  // duplicate = 0; // not a duplicate
		  pnew->duplicate = 0;
		  strcpy(pnew->parkName, parkName);
		  pnew->computerList = init_computerList();

		  pnew->forw = curr;
		  pnew->back = prev;
		  prev->forw = pnew;
		  curr->back = pnew;
	 }

	 return pnew;
}

/***************************************************
	 Traverses forward a circularly doubly-linked
	 list with one sentinel node to print out the
	 contents of each node
*/
void traverse_forw_park(PARK_NODE *parkList, int isWithComputerList)
{
	 putchar('\n');
	 parkList = parkList->forw; // skip the dummy node


	 if(isWithComputerList==0)
	 {
		while (parkList->parkName[0] != DUMMY_TRAILER)
		{
		  printf("\n----Park: %s----", parkList->parkName);
		  parkList = parkList->forw;
		}
	 }else
	 {
		while (parkList->parkName[0] != DUMMY_TRAILER)
		{
		  printf("\n----Park: %s----", parkList->parkName);
		  traverse_forw_computer(parkList->computerList);
		  parkList = parkList->forw;
		}
	 }
	 return;
}

/***************************************************
	 Traverses backward a circularly doubly-linked
	 list with one sentinel node to print out the
	 contents of each node
*/

void traverse_back_park(PARK_NODE *parkList, int isWithComputerList)
{
	 putchar('\n');
	 parkList = parkList->back; // skip the dummy node

	 if(isWithComputerList==0)
	 {
		while (parkList->parkName[0] != DUMMY_TRAILER)
		{
		  printf("\n----Park: %s----", parkList->parkName);
		  parkList = parkList->back;
		}
	 }else
	 {
		while (parkList->parkName[0] != DUMMY_TRAILER)
		{
		  printf("\n----Park: %s----", parkList->parkName);
		  traverse_forw_computer(parkList->computerList);
		  parkList = parkList->back;
		}
	 }
	 return;
}

//Beginning of Computer List - related functions:

/***************************************************
	 Initialization of a circularly doubly-linked
	 list with one sentinel node
*/

COMPUTER_NODE *init_computerList(void)
{
	 COMPUTER_NODE *computerList;

	 // allocate the sentinel node
	 computerList = (COMPUTER_NODE *) malloc(sizeof(COMPUTER_NODE));
	 if (!computerList)
	 {
		  printf("Error in init_computerList!\n");
		  exit(1);
	 }

	 computerList->computerName[0] = DUMMY_TRAILER;
	 computerList->computerName[1] = '\0';
	 computerList->forw = computerList;
	 computerList->back = computerList;

	 return computerList;
}

/***************************************************
	 Insert a node in a un-sorted circularly
	 doubly-linked list with one sentinel node
		  return 1 - if duplicate
		  return 0 - otherwise
*/
int insert_computer(COMPUTER_NODE *computerList, char *computerName, char computerStatus)
{
	COMPUTER_NODE *curr = computerList->forw;
	COMPUTER_NODE *pnew;
	int duplicate = 1;

	while (curr!=computerList)
	{
		if (strcmp(computerName,curr->computerName) == 0)
			return duplicate;
		curr = curr->forw;
	}

	duplicate = 0; // not a duplicate
	pnew = (COMPUTER_NODE *) malloc(sizeof (COMPUTER_NODE));
	if (!pnew)
	{
		printf("Fatal memory allocation error in insert!\n");
		exit(3);
	}
	strcpy(pnew->computerName, computerName);
	pnew->computerStatus = computerStatus;

	pnew->forw = computerList;
	pnew->back = computerList-> back;
	(computerList->back)->forw = pnew;
	computerList -> back =pnew;

	return duplicate;
}

/***************************************************
	 Traverses forward a circularly doubly-linked
	 list with one sentinel node to print out the
	 contents of each node
*/
void traverse_forw_computer(COMPUTER_NODE *computerList)
{
	 putchar('\n');
	 computerList = computerList->forw; // skip the dummy node
	 while (computerList->computerName[0] != DUMMY_TRAILER)
	 {
		  printf("computer: %s, status: %c\n",
		  	computerList->computerName,computerList->computerStatus);
		  computerList = computerList->forw;
	 }
	 return;
}

/***************************************************
	 Traverses backward a circularly doubly-linked
	 list with one sentinel node to print out the
	 contents of each node
*/
void traverse_back_computer(COMPUTER_NODE *computerList)
{
	 putchar('\n');
	 computerList = computerList->back; // skip the dummy node
	 while (computerList->computerName[0] != DUMMY_TRAILER)
	 {
		  printf("computer: %s, status: %c\n",
		  	computerList->computerName,computerList->computerStatus);
		  computerList = computerList->back;
	 }
	 return;
}

//Convert a string to lower case.
char* strlwr(char* s)
{
    char* tmp = s;

    for (;*tmp;++tmp) {
        *tmp = tolower((unsigned char) *tmp);
    }

    return s;
}

//Check whether a string contains only alphabetic characters.
int isAlphaOnly(char *s)
{
    char* tmp = s;

    for (;*tmp;++tmp) 
    {
    	//GCC requires putting parenthesis eventhough they shouldn't be needed.
    	if ((*tmp<'A' && *tmp!=32) || (*tmp>'Z' && *tmp<'a') || *tmp > 'z')
    	{
    		return 0;
    	}
    }

    return 1;
}

//Deallocate memory of the dynamic memory:
void deallocNodes (PARK_NODE* parkList)
{
	PARK_NODE *pPark=parkList->forw;

	while (pPark->parkName[0] != DUMMY_TRAILER)
	{
	  //Start with the node after the sentinel(head) node.
		COMPUTER_NODE *pComputer = pPark->computerList->forw;

		while (pComputer->computerName[0] != DUMMY_TRAILER)
		{
			pComputer = pComputer ->forw;
			free(pComputer->back);
		}
		//Finally free the sentinel node.
		free(pComputer);

		//Move on to the next park
		pPark = pPark->forw;
		free(pPark->back);
	}
	//Finally free the sentinel node.
	free(pPark);
}


/***********Output (including memory leak detection by Valgrind)************

lordofmings-MacBook-Pro:HW-2 AccountName$ gcc -o HW_2_CIS26B HW_2_CIS26B.c
lordofmings-MacBook-Pro:HW-2 AccountName$ valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./HW_2_CIS26B
==8222== Memcheck, a memory error detector
==8222== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
==8222== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
==8222== Command: ./HW_2_CIS26B
==8222== 
--8222-- run: /usr/bin/dsymutil "./HW_2_CIS26B"
warning: no debug symbols in executable (-arch x86_64)

Enter name of input file : 
Input file's name is: comprings.txt

After readFromFile(),all park names without computer list in ascending order (A to Z):

----Park: Big Basin----
----Park: Big Sur----
----Park: Death Valley----
----Park: Devils Postpile----
----Park: Joshua Tree----
----Park: Kings Canyon----
----Park: Lasen Volcanic----
----Park: Pinnacles----
----Park: Point Reyes----
----Park: Redwood----
----Park: Yosemite----

After readFromFile(),all park names in descending order (Z to A) with computer list at each park:

----Park: Yosemite----
computer: caligula, status: 1
computer: theorem, status: 1
computer: star, status: 1
computer: catalyst, status: 0
computer: pixel, status: 0
computer: quantum, status: 1
computer: spock, status: 0
computer: son, status: 0

----Park: Redwood----
computer: turbo, status: 0
computer: voyager, status: 1
computer: crypto, status: 1
computer: magnus, status: 0
computer: nero, status: 0
computer: galileo, status: 0
computer: citadel, status: 1
computer: odin, status: 0
computer: proton, status: 0

----Park: Point Reyes----
computer: amber, status: 0
computer: zion, status: 0
computer: brice, status: 0
computer: oberon, status: 0
computer: cypher, status: 0
computer: venus, status: 1
computer: saturn, status: 0
computer: luna, status: 0
computer: merlin, status: 0
computer: mars, status: 0

----Park: Pinnacles----
computer: eniac, status: 0
computer: khan, status: 1
computer: hal, status: 1
computer: box, status: 0
computer: gizmo, status: 1

----Park: Lasen Volcanic----
computer: zelda, status: 1
computer: flux, status: 1
computer: hydra, status: 1
computer: willow, status: 1
computer: swift, status: 1
computer: aeon, status: 1
computer: neptune, status: 1

----Park: Kings Canyon----
computer: animus, status: 1
computer: xena, status: 0
computer: santiago, status: 1
computer: hex, status: 0
computer: laplace, status: 1
computer: mario, status: 0

----Park: Joshua Tree----
computer: golem, status: 0
computer: pyro, status: 0
computer: unity, status: 0
computer: tardis, status: 0
computer: hawk, status: 0
computer: pluto, status: 0

----Park: Devils Postpile----
computer: minotaur, status: 1
computer: enigma, status: 1
computer: ezra, status: 0
computer: nero, status: 0
computer: reflect, status: 1
computer: canary, status: 1
computer: thor, status: 0
computer: switch, status: 0
computer: mercury, status: 1
computer: loki, status: 1

----Park: Death Valley----
computer: neo, status: 0
computer: thebe, status: 0
computer: titus, status: 0
computer: telescope, status: 1
computer: mosaic, status: 0
computer: zabrinski, status: 0
computer: dante, status: 1
computer: chaos, status: 0
computer: ego, status: 0
computer: vortex, status: 0

----Park: Big Sur----
computer: fusion, status: 1
computer: puma, status: 0
computer: tiger, status: 0
computer: lion, status: 1
computer: rogue, status: 1
computer: aurora, status: 0
computer: link, status: 0

----Park: Big Basin----
computer: hamlet, status: 0
computer: river, status: 1
computer: moose, status: 0
computer: blaze, status: 0
computer: pluto, status: 1
computer: felix, status: 0
computer: genesis, status: 0
computer: amethyst, status: 1

===============================
Enter Park Name: Lasen Volcanic
Enter Computer Name: willow

Read-in of Park Name: Lasen Volcanic | length: 14
Read-in of Computer Name: willow | length: 6

Start Searching...

Search Result:
Both Park and Computer are Found.
The nearest 'up' computer on the left is: hydra, the nearest 'up' computer on the right is: swift.
===============================
Do you want to quit? If so, type quit, otherwise type any key:
n

===============================
Enter Park Name: Devils Postpile
Enter Computer Name: mercury

Read-in of Park Name: Devils Postpile | length: 15
Read-in of Computer Name: mercury | length: 7

Start Searching...

Search Result:
Both Park and Computer are Found.
The nearest 'up' computer on the left is: canary, the nearest 'up' computer on the right is: loki.
===============================
Do you want to quit? If so, type quit, otherwise type any key:
n

===============================
Enter Park Name: Redwood
Enter Computer Name: citadel

Read-in of Park Name: Redwood | length: 7
Read-in of Computer Name: citadel | length: 7

Start Searching...

==8222== Conditional jump or move depends on uninitialised value(s)
==8222==    at 0x100001726: searchNearestUpComputer (in ./HW_2_CIS26B)
==8222==    by 0x1000014E8: searchInputComputer (in ./HW_2_CIS26B)
==8222==    by 0x100000DD7: getUserInputAndSearch (in ./HW_2_CIS26B)
==8222==    by 0x100000802: main (in ./HW_2_CIS26B)
==8222== 
Search Result:
Both Park and Computer are Found.
The nearest 'up' computer on the left is: crypto, no 'up' computer on the right.
===============================
Do you want to quit? If so, type quit, otherwise type any key:
n

===============================
Enter Park Name: Death Valley
Enter Computer Name: dante

Read-in of Park Name: Death Valley | length: 12
Read-in of Computer Name: dante | length: 5

Start Searching...

Search Result:
Both Park and Computer are Found.
The nearest 'up' computer on the left is: telescope, no 'up' computer on the right.
===============================
Do you want to quit? If so, type quit, otherwise type any key:
n

===============================
Enter Park Name: Point Reyes
Enter Computer Name: venus

Read-in of Park Name: Point Reyes | length: 11
Read-in of Computer Name: venus | length: 5

Start Searching...

==8222== Conditional jump or move depends on uninitialised value(s)
==8222==    at 0x10000165B: searchNearestUpComputer (in ./HW_2_CIS26B)
==8222==    by 0x1000014E8: searchInputComputer (in ./HW_2_CIS26B)
==8222==    by 0x100000DD7: getUserInputAndSearch (in ./HW_2_CIS26B)
==8222==    by 0x100000802: main (in ./HW_2_CIS26B)
==8222== 
Search Result:
Both Park and Computer are Found.
No 'up' computer on the left, no 'up' computer on the right.
===============================
Do you want to quit? If so, type quit, otherwise type any key:
n

===============================
Enter Park Name: Pinnacles
Enter Computer Name: mouse

Read-in of Park Name: Pinnacles | length: 9
Read-in of Computer Name: mouse | length: 5

Start Searching...

Search Result:
Park Found but Computer Not Found.

===============================
Do you want to quit? If so, type quit, otherwise type any key:
n

===============================
Enter Park Name: Cupertino
Enter Computer Name: amethyst

Read-in of Park Name: Cupertino | length: 9
Read-in of Computer Name: amethyst | length: 8

Start Searching...

Search Result:
Park Not Found.
===============================
Do you want to quit? If so, type quit, otherwise type any key:
n

===============================
Enter Park Name: lasen volcanic
Enter Computer Name: Willow

Read-in of Park Name: lasen volcanic | length: 14
Read-in of Computer Name: Willow | length: 6

Start Searching...

Search Result:
Both Park and Computer are Found.
The nearest 'up' computer on the left is: hydra, the nearest 'up' computer on the right is: swift.
===============================
Do you want to quit? If so, type quit, otherwise type any key:
n

===============================
Enter Park Name: BIG SUR
Enter Computer Name: rogue 123 tiger

Read-in of Park Name: BIG SUR | length: 7
Read-in of Computer Name: rogue | length: 5

Start Searching...

Search Result:
Both Park and Computer are Found.
The nearest 'up' computer on the left is: lion, no 'up' computer on the right.
===============================
Do you want to quit? If so, type quit, otherwise type any key:
n

===============================
Enter Park Name: BIG 123
Invalid: Park Name should not include digits or any non-letter character.Try again:

===============================
Enter Park Name: BIG SUR
Enter Computer Name: rogue007
Invalid: Computer Name should not include digits or any non-letter character.Start over:

===============================
Enter Park Name: Big 456
Invalid: Park Name should not include digits or any non-letter character.Try again:

===============================
Enter Park Name: big789
Invalid: Park Name should not include digits or any non-letter character.Try again:

===============================
Enter Park Name: big012
Too many failed attempts for park name, exiting the program.

==8222== 
==8222== HEAP SUMMARY:
==8222==     in use at exit: 47,236 bytes in 524 blocks
==8222==   total heap usage: 625 allocs, 101 frees, 58,308 bytes allocated
==8222== 
==8222== 64 bytes in 1 blocks are still reachable in loss record 43 of 83
==8222==    at 0x100008EA1: malloc (vg_replace_malloc.c:303)
==8222==    by 0x100000843: init_parkList (in ./HW_2_CIS26B)
==8222==    by 0x1000007A0: main (in ./HW_2_CIS26B)
==8222== 
==8222== 440 bytes in 11 blocks are still reachable in loss record 59 of 83
==8222==    at 0x100008EA1: malloc (vg_replace_malloc.c:303)
==8222==    by 0x1000017F3: init_computerList (in ./HW_2_CIS26B)
==8222==    by 0x100001048: insert_park (in ./HW_2_CIS26B)
==8222==    by 0x1000009B4: readFromFile (in ./HW_2_CIS26B)
==8222==    by 0x1000007B1: main (in ./HW_2_CIS26B)
==8222== 
==8222== 704 bytes in 11 blocks are still reachable in loss record 65 of 83
==8222==    at 0x100008EA1: malloc (vg_replace_malloc.c:303)
==8222==    by 0x100000FD0: insert_park (in ./HW_2_CIS26B)
==8222==    by 0x1000009B4: readFromFile (in ./HW_2_CIS26B)
==8222==    by 0x1000007B1: main (in ./HW_2_CIS26B)
==8222== 
==8222== 3,440 bytes in 86 blocks are still reachable in loss record 79 of 83
==8222==    at 0x100008EA1: malloc (vg_replace_malloc.c:303)
==8222==    by 0x10000110C: insert_computer (in ./HW_2_CIS26B)
==8222==    by 0x100000A62: readFromFile (in ./HW_2_CIS26B)
==8222==    by 0x1000007B1: main (in ./HW_2_CIS26B)
==8222== 
==8222== 4,096 bytes in 1 blocks are still reachable in loss record 83 of 83
==8222==    at 0x100008EA1: malloc (vg_replace_malloc.c:303)
==8222==    by 0x1001D468E: __smakebuf (in /usr/lib/system/libsystem_c.dylib)
==8222==    by 0x1001D7CF1: __srefill0 (in /usr/lib/system/libsystem_c.dylib)
==8222==    by 0x1001D7DEC: __srefill (in /usr/lib/system/libsystem_c.dylib)
==8222==    by 0x1001D1632: fgets (in /usr/lib/system/libsystem_c.dylib)
==8222==    by 0x10000074E: main (in ./HW_2_CIS26B)
==8222== 
==8222== LEAK SUMMARY:
==8222==    definitely lost: 0 bytes in 0 blocks
==8222==    indirectly lost: 0 bytes in 0 blocks
==8222==      possibly lost: 0 bytes in 0 blocks
==8222==    still reachable: 8,744 bytes in 110 blocks
==8222==         suppressed: 38,492 bytes in 414 blocks
==8222== 
==8222== For counts of detected and suppressed errors, rerun with: -v
==8222== Use --track-origins=yes to see where uninitialised values come from
==8222== ERROR SUMMARY: 5 errors from 2 contexts (suppressed: 16 from 16)

****************************** End of Output ******************************/ 

