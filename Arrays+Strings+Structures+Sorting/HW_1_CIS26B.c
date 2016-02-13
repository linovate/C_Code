/*
**        Arrays, Pointers, Strings, Structures, Sorting, and
**        Dynamic Allocation of Memory
**
********************************************************************************

   Create and process arrays of structures: select students taking 22B & 22C.
 
   Save the output as a comment at the end of the program!
**********************************************************
**
**  Date: 01/13/2016             
**  IDE (compiler): GCC
**  Memory Leak Detection: Valgrind 3.11.0
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
//Added header
#include <string.h>

#ifdef _MSC_VER
#include <crtdbg.h>  // needed to check for memory leaks (Windows only!)
#endif

//Define Student Structure.
typedef struct
{
	int studentID;
	char lastName[31];
	char firstName[31];
} Student;

//Define StudentArray Structure.
typedef struct
{
	Student **ptrStudentArray;
	int sizeOfStudentArray;
} StudentArray;

//Function prototypes:
StudentArray buildArrayFromFile (char inputFileName[]);
void insertionSortStudentArray(StudentArray sArray);
void displayStudentArray(StudentArray sArray);
StudentArray findIntersectionArray (StudentArray sArray1, StudentArray sArray2);
void writeIntersectionArrayToFile (StudentArray intersectionArray,
char output[]);
void deallocStudentArray (StudentArray arr);

int main( void )
{
   //Limit file name to 100 characters.
   char input1[100], input2[100], output[100];

   //Ask for user input and output file name, default file name would be used 
   //if no input.
   printf("Enter name of first input file : ");
   fgets(input1, sizeof(input1), stdin);
   if (input1[0]=='\n')
   {
      strcpy(input1,"in26B.txt");
   }  
   printf("1st input file is: %s\n",input1);

   printf("Enter name of second input file : ");
   fgets(input2, sizeof(input2), stdin);
   if (input2[0]=='\n')
   {
      strcpy(input2,"in22C.txt");
   }
   printf("2nd input file is:%s\n",input2);

   printf("Enter name of output file : ");
   fgets(output, sizeof(output), stdin);
   if (output[0]=='\n')
   {
      strcpy(output,"output.txt");
   } 
   printf("output file is:%s\n",output);

	StudentArray array1= buildArrayFromFile (input1);
	insertionSortStudentArray(array1);
	printf ("\n-------Array for 1st file after insertion sort:-------\n"); 
	displayStudentArray(array1);

	StudentArray array2= buildArrayFromFile (input2);
	insertionSortStudentArray(array2);
	printf ("\n-------Array for 2nd file after insertion sort:-------\n"); 
	displayStudentArray(array2);

	StudentArray intersectionArray = findIntersectionArray (array1, array2);
	printf ("\n-------Intersection Array:-------\n"); 
	displayStudentArray(intersectionArray); 

	writeIntersectionArrayToFile (intersectionArray,output);

	deallocStudentArray (array1);
	deallocStudentArray (array2);
	deallocStudentArray (intersectionArray);

   //If use Microsoft Visual Studio as IDE:
    // #ifdef _MSC_VER
    // printf( _CrtDumpMemoryLeaks() ? "Memory Leak\n" : "No Memory Leak\n");
    // #endif

   //Use Valgrind to detect memory leak instead.

    return 0;
}


//Beginning of function definitions:

//Building and populate Student Array based on input from file.
StudentArray buildArrayFromFile (char inputFileName [])
{

   FILE *inputFile = fopen(inputFileName, "r");

   if (inputFile == NULL)
   {
      printf ("Error opening file for reading.");
      exit(101);  
   }

   int numberOfStudentsInFile;
   int studentIDReadin;
   char studentLastNameReadin[31];
   char studentFirstNameReadin[31];

   fscanf (inputFile, "%d", &numberOfStudentsInFile);

   //Dynamically allocate memory for array of pointers for 1st array.
   Student **arrayForFile = (Student **) malloc
   (numberOfStudentsInFile * sizeof(Student*));
   if(arrayForFile == NULL)
   {
      printf ("MEM_ERROR."); 
      exit(100);     
   }
   //Dynamically allocate memory for each element of Student pointer array.
   for (int i = 0; i < numberOfStudentsInFile; i++)
   {
      arrayForFile[i]=(Student *) malloc(1 * sizeof(Student));
      if(arrayForFile[i] == NULL)
      {
         printf ("MEM_ERROR."); 
         exit(100);     
      }
   }

   //Populate the array with data read in from file.
	int entry=0;
   while ((fscanf(inputFile, "%d %s %s", &studentIDReadin, 
      studentLastNameReadin, studentFirstNameReadin))!=EOF)
   {
   	arrayForFile[entry]->studentID=studentIDReadin;
   	strcpy(arrayForFile[entry]->lastName,studentLastNameReadin);
   	strcpy(arrayForFile[entry]->firstName,studentFirstNameReadin);

   	entry++;
   } 

   //Close input file,print error if there is any.
   if (fclose(inputFile) == EOF)
   {
      printf ("\nError closing file!\n");
      exit(201); 
   }

   StudentArray retArray = {arrayForFile, numberOfStudentsInFile};

   return retArray;
}

//Find common Students in two given StudentArrays.
StudentArray findIntersectionArray (StudentArray sArray1,StudentArray sArray2)
{
   int sizeOfIntersectionArray = 0; 
 	StudentArray shorterArray, longerArray;

	if (sArray1.sizeOfStudentArray < sArray2.sizeOfStudentArray)
	{
		shorterArray = sArray1;
		longerArray = sArray2;
		sizeOfIntersectionArray = sArray1.sizeOfStudentArray;
	}else
	{
		shorterArray = sArray2;
		longerArray = sArray1;
		sizeOfIntersectionArray = sArray2.sizeOfStudentArray;
	}

	Student **intersectionArray = (Student **) malloc
   (sizeOfIntersectionArray * sizeof(Student*));

   if(intersectionArray == NULL)
   {
      printf ("MEM_ERROR."); 
      exit(100);     
   }

   //Dynamically allocate memory for each element of Student pointer array.
   for (int i = 0; i < sizeOfIntersectionArray; i++)
   {
      intersectionArray[i]=(Student *) malloc(1 * sizeof(Student));
      if(intersectionArray[i] == NULL)
      {
         printf ("MEM_ERROR."); 
         exit(100);     
      }
   }

   //Finding the intersection of the first 2 arrays and populate it in the 
   //intersection array, it's sorted by design.
   int lengthOfIntersectionArray = 0;
   int startIndexInLongerArray = 0;
   for (int i=0; i< shorterArray.sizeOfStudentArray; i++)
   {
      for (int j=startIndexInLongerArray; j< longerArray.sizeOfStudentArray;j++)
      {
         if((longerArray.ptrStudentArray[j]->studentID) == 
         	(shorterArray.ptrStudentArray[i]->studentID))
         {
            intersectionArray[lengthOfIntersectionArray]->studentID
            =shorterArray.ptrStudentArray[i]->studentID;

            strcpy(intersectionArray[lengthOfIntersectionArray]->lastName
               ,shorterArray.ptrStudentArray[i]->lastName);

            strcpy(intersectionArray[lengthOfIntersectionArray]->firstName
               ,shorterArray.ptrStudentArray[i]->firstName);

            lengthOfIntersectionArray++;
            startIndexInLongerArray=j+1;
            break;
         }
      }
   }

   if (lengthOfIntersectionArray < sizeOfIntersectionArray)
   {
   	//Free the memory allocated to those over-allocated Student pointers.
   	for (int i = lengthOfIntersectionArray; i < sizeOfIntersectionArray; i++)
   	{
   		free (intersectionArray[i]);
   	}
   	//Shrink the intersection array to free up unnecessary memory.
   	Student **reallocedIntersectionArray = 
   	(Student **) realloc(intersectionArray, 
   		lengthOfIntersectionArray*sizeof(Student*));

   	if (reallocedIntersectionArray == NULL)
   	{
   		printf("MEM_ERROR");
   		exit(100);
   	}else
   	{
   		StudentArray retArray = {reallocedIntersectionArray,
   		 lengthOfIntersectionArray};
   		return retArray;
   	}

   }else
   {
   	return shorterArray;
   }
}

//Display all the students in a StudentArray.
void displayStudentArray(StudentArray sArray)
{
	Student **arr = sArray.ptrStudentArray;

   for (int i=0;i<sArray.sizeOfStudentArray;i++)
   {
      printf("%d %s%s\n",arr[i]->studentID,arr[i]->lastName
         ,arr[i]->firstName);
   }
}

//Insertion sort the array of Student pointers - (more efficient).
void insertionSortStudentArray(StudentArray sArray)
{
	Student **arr = sArray.ptrStudentArray;
	int arraySize = sArray.sizeOfStudentArray;

   int i = 0;
   int j = 0;

   for (i = 1; i < arraySize; i++) 
   {
      Student *temp = arr[i];
      j = i - 1;
      while (j >= 0 && temp->studentID < arr[j]->studentID)
      {
         arr[j + 1] = arr[j];
         j = j - 1;
      }
      arr[j + 1] = temp;
   }
}

//Write intersection array to an output file in the same format as input file.
void writeIntersectionArrayToFile(StudentArray intersectionArray, char output[])
{
   FILE *outputFile;
   outputFile = fopen(output, "w");
   if (outputFile == NULL)
   {
      printf ("Error opening output file for writing.");
      exit(101);  
   }else
   {
      fprintf(outputFile,"%d\n",intersectionArray.sizeOfStudentArray);
      for (int i = 0;i < intersectionArray.sizeOfStudentArray;i++)
      {
         fprintf(outputFile,"%d %s %s\n",
         	(intersectionArray.ptrStudentArray)[i]->studentID,
            (intersectionArray.ptrStudentArray)[i]->lastName,
            (intersectionArray.ptrStudentArray)[i]->firstName);
      }
      //Close output file,print error if there is any.
	   if (fclose(outputFile) == EOF)
	   {
	      printf ("\nError closing file(s)!\n");
	      exit(201); 
	   }
   }
}

//Deallocate memory of the dynamic array in a StudentArray struct:
void deallocStudentArray (StudentArray sArray)
{
	//Deallocate memory allocated to each Student pointer.
   for (int i = 0; i < sArray.sizeOfStudentArray; i++)
   {
      free((sArray.ptrStudentArray)[i]);
   }
   //Deallocate memory allocated to array of pointers.
   free(sArray.ptrStudentArray);
}

/***********Output (including memory leak detection by Valgrind)************
lordofmings-MacBook-Pro:26B_Li_He_H1 AccountName$ 
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./HW_1_CIS26B
==2404== Memcheck, a memory error detector
==2404== Copyright (C) 2002-2015, and GNU GPL'd, by Julian Seward et al.
==2404== Using Valgrind-3.11.0 and LibVEX; rerun with -h for copyright info
==2404== Command: ./HW_1_CIS26B
==2404== 
Enter name of first input file : 
1st input file is: in26B.txt
Enter name of second input file : 
2nd input file is:in22C.txt
Enter name of output file : 
output file is:output.txt

-------Array for 1st file after insertion sort:-------
1189 Shmoys,David
1234 Marley,Tom
2901 Green,Mary
2908 Vigoda,Eric
3456 Karlin,Anna
4344 Kelley,Sandra
5445 Homer,Steve
5567 Welch,Jennifer
6566 Williams,Ryan
6579 Vadhan,Salil
6766 Hemaspaandra,Lane
8372 Chen,Li
8433 Chakrabarti,Amit
8879 Bein,Wolfgang
8999 Fenner,Mia
9002 Khuller,Samira
9123 Vianu,Victor
9865 Beame,Paul

-------Array for 2nd file after insertion sort:-------
1111 Tan,Li-Yang
2000 Barenboim,Leonid
2001 Rossman,Marie
3456 Karlin,Anna
4344 Kelley,Sandra
5445 Homer,Steve
5511 Welch,Claire
6009 Mumey,Brendan
6577 Green,Susan
6666 Forbes,Michael
8433 Chakrabarti,Amit
8800 Servedio,Rocco
8999 Fenner,Mia
9123 Vianu,Victor
9865 Beame,Paul

-------Intersection Array:-------
3456 Karlin,Anna
4344 Kelley,Sandra
5445 Homer,Steve
8433 Chakrabarti,Amit
8999 Fenner,Mia
9123 Vianu,Victor
9865 Beame,Paul
==2404== 
==2404== HEAP SUMMARY:
==2404==     in use at exit: 42,588 bytes in 415 blocks
==2404==   total heap usage: 570 allocs, 155 frees, 65,556 bytes allocated
==2404== 
==2404== 4,096 bytes in 1 blocks are still reachable in loss record 79 of 79
==2404==    at 0x100008EA1: malloc (vg_replace_malloc.c:303)
==2404==    by 0x1001D468E: __smakebuf (in /usr/lib/system/libsystem_c.dylib)
==2404==    by 0x1001D7CF1: __srefill0 (in /usr/lib/system/libsystem_c.dylib)
==2404==    by 0x1001D7DEC: __srefill (in /usr/lib/system/libsystem_c.dylib)
==2404==    by 0x1001D1632: fgets (in /usr/lib/system/libsystem_c.dylib)
==2404==    by 0x1000010F1: main (in ./HW_1_CIS26B)
==2404== 
==2404== LEAK SUMMARY:
==2404==    definitely lost: 0 bytes in 0 blocks
==2404==    indirectly lost: 0 bytes in 0 blocks
==2404==      possibly lost: 0 bytes in 0 blocks
==2404==    still reachable: 4,096 bytes in 1 blocks
==2404==         suppressed: 38,492 bytes in 414 blocks
==2404== 
==2404== For counts of detected and suppressed errors, rerun with: -v
==2404== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 16 from 16)

****************************** End of Output ******************************/ 

