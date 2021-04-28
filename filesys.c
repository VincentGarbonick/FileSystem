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
	printf("Format: ./yourfile.out L\n\n");

	printf("D: Deletes files\n");
	printf("Format: ./yourfile.out D filename\n");
	printf("DO NOT ADD EXTENSION TO FILENAME\n\n");

	printf("P: Prints files\n");
	printf("Format: ./yourfile.out P filename\n");
	printf("DO NOT ADD EXTENSION TO FILENAME\n\n");
		
	printf("M: Creates and stores file\n");
	printf("Format: ./yourfile.out M filename\n");
	printf("DO NOT ADD EXTENSION TO FILENAME\n\n");

	return;
}

int main(int argc, char* argv[])
{
	// conditional for covering minimal and maximal amount of arguments for each command
	if((*argv[1] == 'L' && argc != 2)
		|| (*argv[1] == 'P' && argc != 3)
		|| (*argv[1] == 'M' && argc != 3)
		|| (*argv[1] == 'Z' && argc != 3))
	{
		printf("Invalid argument number, check accepted argument formats for proper command calling\n");
		helpScreen();
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
		{
			dir[i]=fgetc(floppy);
		}
		// if-else for user input 
		if(*argv[1] == 'L')
		{
			int byteTotal = 0; // integer for the total number of bytes 

			// print directory
			printf("Disk directory:\n");
			// starting at zero, moving through sector in 16 bit chunks 
			for (i=0; i<512; i=i+16) 
			{
				// stops when we hit our first zero, because everything is 
				// allocated continguously 
				if (dir[i]==0) break;
				
				// moving through first byte bit by bit...first byte is 
				// name 
				for (j=0; j<8; j++) 
				{
					//if (dir[i+j]==0) printf(" "); else printf("%c",dir[i+j]);
					if(dir[i+j] != 0)
					{
						printf("%c", dir[i+j]);
					}
				}
			 	
					
				// moves through second byte bite by bit, this is padded with 
				// seven zeroes and then a t or x at the end

				if(dir[i+8] == 't')
				{
					printf(".t");
				}
				else if(dir[i+8] == 'x')
				{
					printf(".x");
				}

				printf("\t");

				//printf(" %5d %6d bytes\n", dir[i+9], 512*dir[i+10]);
				
				// add the space taken up by this file to total taken space

				byteTotal = byteTotal + 512*dir[i+10];
			}
			printf("\nFree space: %iB\n", 261632 - byteTotal);
			printf("Space used: %i/261632B\n", byteTotal);
		}
		else if(*argv[1] == 'P')
		{
			int argumentSize = 0;
			int j = 0;
			
			int fileNameSize = 0;
			int foundFlag = 0;

			int foundIndex = 0;
			// get size of our input filename 
			while(argv[2][j] != '\0')
			{	
				argumentSize++; 
				j++;	
			}	

			// go through directory trying to match file name without extension
			for(int i = 0; i < 512; i += 16)
			{
			
				// stop when we hit our first zero, because everything is allocated contiguously 
				// also stop if we found where the word was 
				if(dir[i] == 0 || foundFlag == 1) break;
				// go through each non null terminated letter of our input arg
					
				j = 0; // counting variable for going through argv 
				fileNameSize = 0;	
					
				// check size of current name 
				for(int k = 0; k < 8; k++)
				{
					if(dir[i+k] != 0)
					{
						fileNameSize++;
					}
					else 
					{
						break;
					}	
				}
				// if the names are the same size
				if(argumentSize == fileNameSize)
				{
					for(int z = 0; z < fileNameSize; z++)
					{
						if(dir[i+z] != argv[2][z])
						{
							foundFlag = 0; 
							foundIndex = 0;
							break;	
						}
						else 
						{
							foundIndex = i;
							foundFlag = 1;
						}
					}
				}

				if(foundFlag == 1)
				{
					break;
				}
			}
			// if we manage to find the flag and we find the contents of file...
			if(foundFlag == 1) 
			{
				// we must check if the file is executable...if it is an exe we must not print it 
				if(dir[foundIndex + 8] == 'x')
				{
					printf("ERROR: FILE IS NOT A TEXT FILE\n");
					return 0;
				}

				// since we know the file is in the directory
				// we must get the start sector
				int startSector = dir[foundIndex + 9];
				// and sector size 
				int sectorLength = dir[foundIndex + 10];
				
				char contents[512 * sectorLength]; // define a character buffer

				fseek(floppy, startSector*512, SEEK_SET); // locate sector where file is

				for(int m = 0; m < (512 * sectorLength); m++)
				{
					contents[m] = fgetc(floppy);
				}
				
				int n = 0; 
				while(contents[n] != 0) 
				{
					printf("%c", contents[n]);
					n++;

				}


			}
			else
			{
				printf("FILE NOT FOUND\n");
			}


		}
		else if(*argv[1] == 'M')
		{

			int argumentSize = 0;
			int j = 0;
			
			int fileNameSize = 0;
			int foundFlag = 0;

			int foundIndex = 0;
			// get size of our input filename 
			while(argv[2][j] != '\0')
			{	
				argumentSize++; 
				j++;	
			}	

			// go through directory trying to match file name without extension
			for(int i = 0; i < 512; i += 16)
			{
			
				// stop when we hit our first zero, because everything is allocated contiguously 
				// also stop if we found where the word was 
				if(dir[i] == 0 || foundFlag == 1) break;
				// go through each non null terminated letter of our input arg
					
				j = 0; // counting variable for going through argv 
				fileNameSize = 0;	
					
				// check size of current name 
				for(int k = 0; k < 8; k++)
				{
					if(dir[i+k] != 0)
					{
						fileNameSize++;
					}
					else 
					{
						break;
					}	
				}
				// if the names are the same size
				if(argumentSize == fileNameSize)
				{
					for(int z = 0; z < fileNameSize; z++)
					{
						if(dir[i+z] != argv[2][z])
						{
							foundFlag = 0; 
							foundIndex = 0;
							break;	
						}
						else 
						{
							foundIndex = i;
							foundFlag = 1;
						}
					}
				}

				if(foundFlag == 1)
				{
					break;
				}
			}
			// we found out the file already exists 
			if(foundFlag == 1) 
			{
				printf("Error: file name already exists...\n");
			}
			// the file does not exist, so we can begin writing
			else
			{
				// TODO: prompt user for string 
				int freeDirectorySpot = 0;
				int foundFreeSpot = 0;

				// find free spot to put name of new file in directory				
				for(int p = 0; p < 512; p += 16)
				{

					// move in 16 bit chunks 
					if(dir[p] == 0)
					{
						freeDirectorySpot = p; 
						foundFreeSpot = 1;
						break;
					}
				}
				
				// if we cound't find a free spot, exit program 
				if(foundFreeSpot == 0)
				{
					printf("Not enough space!\n");
					return 0;
				}

				// put name into new free space 
				int padCount = 0;
				
				// copy name down 
				while(padCount < 8)
				{
					if(argv[2][padCount] == '\0') // if we hit a null terminator, break away and start padding with zeroes
					{
						break;
					}
					else 
					{
						dir[freeDirectorySpot + padCount] = argv[2][padCount];
						padCount++;
					}
				}
				// if we have space remaining
				if(padCount != 7)
				{
					while(padCount < 8)
					{
						dir[freeDirectorySpot + padCount] = 0;
						padCount++;
					}
				}

				// append 't' to the 9th spot to indicate this is a text file 
				dir[freeDirectorySpot + 8] = 't';

				// apend '1' for sector lenght, because we KNOW our text file will only be about 512b at most 
				dir[freeDirectorySpot + 10] = 1;

				// search for a free spot in the map 
				int foundMapFreeSpot = 0; // boolean variable for finding map spot 
				int mapFreeSpot = 0; // location of free spot in map
				
				for(int i = 0; i < 512; i++)
				{
					if(map[i] == 0)
					{
						mapFreeSpot = i;
						foundMapFreeSpot = 1;
						map[mapFreeSpot] = 255; // set the entry in map to 255
						break;
					}
				}
				
				if(foundMapFreeSpot == 0)
				{
					printf("Did not find free spot in map...\n");
					return 0;
				}

				// write free spot in directory
				dir[freeDirectorySpot + 9] = mapFreeSpot;


				// prompt user for a string 
				printf("Enter a message to store: ");
				
				char msg[512];
				fgets(msg, sizeof(msg), stdin);
				
				// since we know the file is in the directory
				// we must get the start sector
				int startSector = dir[freeDirectorySpot + 9];

				fseek(floppy, startSector*512, SEEK_SET); // locate sector where file is
				for (i=0; i<512; i++) fputc(msg[i],floppy); // write to map 

				int sectorLength = dir[freeDirectorySpot + 10];
			}


		}
		else if(*argv[1] == 'Z')
		{
			//print disk map
			printf("Disk usage map:\n");
			printf("      0 1 2 3 4 5 6 7 8 9 A B C D E F\n");
			printf("     --------------------------------\n");
			for (i=0; i<16; i++) {
				switch(i) {
					case 15: printf("0xF_ "); break;
					case 14: printf("0xE_ "); break;
					case 13: printf("0xD_ "); break;
					case 12: printf("0xC_ "); break;
					case 11: printf("0xB_ "); break;
					case 10: printf("0xA_ "); break;
					default: printf("0x%d_ ", i); break;
				}
				for (j=0; j<16; j++) {
					if (map[16*i+j]==-1) printf(" X"); else printf(" .");
				}
				printf("\n");
			}

    		// print directory
			printf("\nDisk directory:\n");
			printf("Name    Type Start Length\n");
    		for (i=0; i<512; i=i+16) {
				if (dir[i]==0) break;
				for (j=0; j<8; j++) {
					if (dir[i+j]==0) printf(" "); else printf("%c",dir[i+j]);
				}
				if ((dir[i+8]=='t') || (dir[i+8]=='T')) printf("text"); else printf("exec");
				printf(" %5d %6d bytes\n", dir[i+9], 512*dir[i+10]);
			}
		}
		else if(*argv[1] == 'D')
		{
			

			int argumentSize = 0;
			int j = 0;
			
			int fileNameSize = 0;
			int foundFlag = 0;

			int foundIndex = 0;
			// get size of our input filename 
			while(argv[2][j] != '\0')
			{	
				argumentSize++; 
				j++;	
			}	

			// go through directory trying to match file name without extension
			for(int i = 0; i < 512; i += 16)
			{
			
				// stop when we hit our first zero, because everything is allocated contiguously 
				// also stop if we found where the word was 
				if(dir[i] == 0 || foundFlag == 1) break;
				// go through each non null terminated letter of our input arg
					
				j = 0; // counting variable for going through argv 
				fileNameSize = 0;	
					
				// check size of current name 
				for(int k = 0; k < 8; k++)
				{
					if(dir[i+k] != 0)
					{
						fileNameSize++;
					}
					else 
					{
						break;
					}	
				}
				// if the names are the same size
				if(argumentSize == fileNameSize)
				{
					for(int z = 0; z < fileNameSize; z++)
					{
						if(dir[i+z] != argv[2][z])
						{
							foundFlag = 0; 
							foundIndex = 0;
							break;	
						}
						else 
						{
							foundIndex = i;
							foundFlag = 1;
						}
					}
				}

				if(foundFlag == 1)
				{
					break;
				}





		}
		else 
		{
			printf("Invalid argument, here is a list of valid arguments...\n");
			helpScreen();
		}

		
		//write map back 
		fseek(floppy,512*256,SEEK_SET);
		for (i=0; i<512; i++) fputc(map[i],floppy);

		//write dir back
		fseek(floppy,512*257,SEEK_SET);
		for (i=0; i<512; i++) fputc(dir[i],floppy);
		

		fclose(floppy);
	
	}
	return 0;

	}
