// ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed:_____________________________________ Date:_____________

//filesys.c
//Based on a program by Michael Black, 2007
//Revised 11.3.2020 O'Neil

#include <stdio.h>


// Prints the help screen 
// Precondition: none
// Postcondition: none 
void helpScreen()
{
	printf("L: Lists files\n");
	printf("D: Deletes files\n");
	printf("P: Prints files\n");
	printf("M: Creates and stores file\n");
	
	return;
}

int main(int argc, char* argv[])
{
	if(argc > 2)
	{
		printf("Too many arguments, only accepts one argument\n");
		return 0;
	}
	else if(argc == 1)
	{

		printf("Please specify an argument...\n");
		helpScreen();
		return 0;
	}
	else 
	{
		int i, j, size, noSecs, startPos;

		//open the floppy image
		FILE* floppy;
		floppy=fopen("floppya.img","r+"); // basic file open
		if (floppy==0) // terminate program if fopen returns 0
		{
			printf("floppya.img not found\n");
			return 0;
		}

		//load the disk map from sector 256
		char map[512]; // a single sector, each portion corresponding to a sector

		/*
		 * fseek moves file pointer position to given location
		 * fseek(FILE *fp, long int offset, int whence 
		 *
		 * fp - file pointer 
		 * offset- where to move from whence or current fp position 
		 * whence- current file pointer position 
		 * 
		 * SEEK_SET- move fp to beginning of fiel 
		 *
		 */

		fseek(floppy,512*256,SEEK_SET); // this position is for the map (256 = map)
		for(i=0; i<512; i++)
			map[i]=fgetc(floppy); // gets next character from stream

		//load the directory from sector 257
		char dir[512];
		fseek(floppy,512*257,SEEK_SET);
		for (i=0; i<512; i++)
			dir[i]=fgetc(floppy);
		// if-else for user input 
		if(*argv[1] == 'L')
		{	
			// print directory
			printf("\nDisk directory:\n");
			printf("Name    Type Start Length\n");
			for (i=0; i<512; i=i+16) 
			{
				if (dir[i]==0) break;
				for (j=0; j<8; j++) {
					if (dir[i+j]==0) printf(" "); else printf("%c",dir[i+j]);
				}
				if ((dir[i+8]=='t') || (dir[i+8]=='T')) printf("text"); else printf("exec");
				printf(" %5d %6d bytes\n", dir[i+9], 512*dir[i+10]);
			}

		}
		else 
		{
			printf("Invalid argument, here is a list of valid arguments...\n");
			helpScreen();
		}

		/*
		//write the map and directory back to the floppy image
		fseek(floppy,512*256,SEEK_SET);
		for (i=0; i<512; i++) fputc(map[i],floppy);

		fseek(floppy,512*257,SEEK_SET);
		for (i=0; i<512; i++) fputc(dir[i],floppy);
		*/

		//printf("%s", argv[1]);
		fclose(floppy);
	
	}
	return 0;

	}
