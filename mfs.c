/*
  Name:             ID:
  Carlos Meza       1001170229
  Matt Crum         1001077895
*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>

// Split up command lines into tokens from every whitespace
#define WHITESPACE " \t\n"
// The maximum command-line size
#define MAX_COMMAND_SIZE 255
// Mfs command prompt prompt supports max of 5 arguments
#define MAX_NUM_ARGUMENTS 5

#define BPB_BytesPerSec_Offset 11
#define BPB_BytesPerSec_Size 2

#define BPB_SecPerClust_Offset 13
#define BPB_SecPerClust_Size 1

#define BPB_RsvdSecCnt_Offset 14
#define BPB_RsvdSecCnt_Size 2

#define BPB_NumFATs_Offset 16
#define BPB_NumFATs_Size 1

#define BPB_RootEntCnt_Offset 17
#define BPB_RootEntCnt_Size 2

#define BPB_FATz32_Offset 36
#define BPB_FATz32_Size 4

#define BS_VolLab_Offset 71
#define BS_VolLab_Size 11

int status = 0;
FILE *fp;

struct __attribute__((__packed__)) DirectoryEntry
{
    char DIR_Name[11];
    uint8_t Dir_Attr;
    uint8_t Unused1[8];
    uint16_t DIR_FirstClusterHigh;
    uint8_t Unused[4];
    uint16_t DIR_FirstClusterLow;
    uint32_t DIR_FileSize;
};

struct DirectoryEntry dir[16];

uint16_t BPB_BytesPerSec;
uint8_t BPB_SecPerClust;
uint16_t BPB_RsvdSecCnt;
uint8_t BPB_NumFATs;
uint32_t BPB_FATz32;
uint16_t BPB_RootEntCnt;
uint32_t RootClusAddress;
char BS_VolLab[11];

/*
 *Function      : nextLb
 *Parameters    : The current sector number that points to a block of data
 *Returns       : The next sector number that points to the next block of data
 *Description   : Finds the sector number of the next block of data given the initial block of data's sector number
*/
int16_t nextLB(uint32_t sector)
{
    uint32_t FATAddress = ( BPB_BytesPerSec * BPB_RsvdSecCnt) + (sector*4);
    int16_t val;
    fseek( fp, FATAddress, SEEK_SET );
    fread( &val, 2, 1, fp);
    return val;

}

/*
 *Function      : LBAToOffset
 *Parameters    : The current sector number that points to a block of data
 *Returns       : The Value of the address for that block of data
 *Description   : Finds the starting address of a block of data given the sector number corresponding to that data block.
*/
int LBAToOffset(int32_t sector)
{
    return (( sector -2 )* BPB_BytesPerSec) + (BPB_BytesPerSec * BPB_RsvdSecCnt) + (BPB_NumFATs * BPB_FATz32 * BPB_BytesPerSec);
}

/*
*Function       : fileName
*Parameters     : The file name inputed by the user
*Returns        : The correctly formated name of the file name
*Description    : Takes in file name string and returns the correct format of name to search by
*/
void fileName(char final[100], char * file)
{
    char name[30];
    char temp[50];
    char ext[30];
    char *mid = "        ";
    char * token;
    // Split up string given by user up to the first period then copy the 
    // 8 spaces in between before adding the file type at the end of it
    strcpy(temp, file);
    token = strtok(temp, ".");
    strcpy(name, token);
    token = strtok(NULL, ".");
    strcpy(ext, token);
    
    strcpy(final, name);
    strcat(final, mid);
    strcat(final, ext);
    // While loop to iterate over each character and uppercase it using
    // the toupper build in function
    while(*final)
    {
        *final = toupper((unsigned char)*final);
        final++;
    }
}

/*

*/
void open()
{
    // BPB_BytesPerSec
    fseek(fp, BPB_BytesPerSec_Offset, SEEK_SET);
    fread(&BPB_BytesPerSec, BPB_BytesPerSec_Size, 1, fp);

    // BPB_secPerClus
    fseek(fp, BPB_SecPerClust_Offset, SEEK_SET);
    fread(&BPB_SecPerClust, BPB_SecPerClust_Size, 1, fp);

    // BPB_RsvdSecCnt
    fseek(fp, BPB_RsvdSecCnt_Offset, SEEK_SET);
    fread(&BPB_RsvdSecCnt, BPB_RsvdSecCnt_Size, 1, fp);

    // BPB_NumFATs
    fseek(fp, BPB_NumFATs_Offset, SEEK_SET);
    fread(&BPB_NumFATs, BPB_NumFATs_Size, 1, fp);

    // BPB_RootEntCnt
    fseek(fp, BPB_RootEntCnt_Offset, SEEK_SET);
    fread(&BPB_RootEntCnt, BPB_RootEntCnt_Size, 1, fp);

    // BPB_FATz32
    fseek(fp, BPB_FATz32_Offset, SEEK_SET);
    fread(&BPB_FATz32, BPB_FATz32_Size, 1, fp);

    // BS_VolLab
    fseek(fp, BS_VolLab_Offset, SEEK_SET);
    fread( &BS_VolLab, BS_VolLab_Size, 1, fp);
}

/*
*Function       : Info
*Parameters     : None
*Returns        : Prints out information on cluster
*Desription     : Prints out information on global variables of cluster attributes
*/
void info()
{
        // Bytes per sector in Base 10 and Hex 
        printf("\n\nBPB_BytesPerSec : %d\n", BPB_BytesPerSec);
        printf("BPB_BytesPerSec : %x\n\n", BPB_BytesPerSec);
        // Sectors per cluster in Base 10 and Hex
        printf("BPB_SecPerClus : %d\n",BPB_SecPerClust);
        printf("BPB_SecPerClus : %x\n\n", BPB_SecPerClust);
        // Rsvd in Base 10 and Hex
        printf("BPB_RsvdSecCnt : %d\n", BPB_RsvdSecCnt);
        printf("BPB_RsvdSecCnt : %x\n\n", BPB_RsvdSecCnt);
        // NumFats in Base 10 and Hex
        printf("BPB_NumFATs : %d\n", BPB_NumFATs);
        printf("BPB_NumFATs : %x\n\n", BPB_NumFATs);
        // FATsz32 in Base 10 and Hex
        printf("BPB_FATsz32 : %d\n", BPB_FATz32);
        printf("BPB_FATsz32 : %x\n\n", BPB_FATz32);
}


int main()
{
    char file_name[100];
    char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

    while(1)
    {
        // Print out the mfs prompt
        printf("mfs> ");
        // Read the command from command line
        // The maximum command that will be read is MAX_COMMAND_SIZE
        // This while command will wait until user inputs something since
        // fgets returns NULL when there is no input
        while(!fgets(cmd_str, MAX_COMMAND_SIZE, stdin));
        //Parse input
        char *token[MAX_NUM_ARGUMENTS];
        int token_count = 0;
        // Pointer to point to the token parsed byt strsep
        char *arg_ptr;
        char *working_str = strdup(cmd_str);
        // Move working_str pointer to keep track of its original value
        // to deallocate correct amoun at the end
        char *working_root = working_str;
        // Tokenize input strings with whitespace and as the delimeter
        while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
            (token_count<MAX_NUM_ARGUMENTS))
        {
        token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
        if( strlen( token[token_count] ) == 0 )
        {
            token[token_count] = NULL;
        }
            token_count++;
        }

        // Handles blank input from user, if nothing entered then continue
        if(token[0] == NULL)
        {
            continue;
        }
        ////////  close  ////////
        // close fat32 image, if not open then output error. Any command
        // after close (except "open") shall result in error
        else if(strcmp(token[0], "close")==0)
        {
            if(status == 1)
            {
                fclose(fp);
                status = 0;
            }
            else
            {
                printf("Error: File system not open.\n");
            }
        }
        else if(strcmp(token[0], "quit")==0)
        {
            exit(1);
        }
        ////////  open  ////////
        // Open fat32 image, filenames shall not contain spaces and limited
        else if(strcmp(token[0], "open")==0)
        {
            if(status == 0)
            {
                fp = fopen( "fat32.img", "r");
                status = 1;
                if( fp == NULL )
                {
                    perror("Error: File system image not found.\n");
                }
                open();
                // Calculating the address of the root directory
                RootClusAddress = (BPB_NumFATs * BPB_FATz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt * BPB_BytesPerSec);
                fseek( fp, RootClusAddress, SEEK_SET );
            }
            else
            {
                printf("Error: File system image already open.\n");
            }
        }
        ////////  info  ////////
        // Command to print out information about file system in hexadecimal and base 10
        else if(strcmp(token[0], "info")==0)
        {
            if(status == 1)
            {
                info();
            }
            else
            {
                printf("Error: File system image must be opened first\n");
            }
        }
        ////////  stat  ////////
        // Print out attributes and starting cluster numbers of file or directory name
        // If parameter is directory name, then size == 0. If name does not exits then output error
        else if(strcmp(token[0], "stat")==0)
        {
            if(status == 1)
            {
                printf("Attribute     Size    Sarting Cluser Number\n");
                printf("%d            %d      %d");
            }
            else
            {
                printf("Error: FIle system image must be opened first.\n");
            }
        }
        ////////  get  ////////
        // Retrieve file from fat 32 image and place it in your current working directory.
        // If the file or directory does not exist then output error
        else if(strcmp(token[0], "get")==0)
        {
            if(status == 1)
            {
                printf("get working.\n");
            }
            else
            {
                printf("Error: FIle system image must be opened first.\n");
            }
        }
        ////////  cd  ////////
        // Change the current working directory to the given directory. Supports relative
        // paths (cd ../name) and absolute paths
        else if(strcmp(token[0], "cd")==0)
        {
            if(status == 1)
            {
                printf("cd working.\n");
            }
            else
            {
                printf("Error: FIle system image must be opened first.\n");
            }
        }
        ////////  ls  ////////
        // Lists the directory contents. Supports listing "." and ".."
        // Does not list deleted files or system volume names
        else if(strcmp(token[0], "ls")==0)
        {
            if(status == 1)
            {
                int i;
                for( i = 0; i < 16; i++ )
                {
                    fread( &dir[i], sizeof( struct DirectoryEntry ), 1, fp );
                }
                // printing how clusters are layed out
                for( i = 0; i < 16; i++)
                {
                    char name[12];
                    memcpy( name, dir[i].DIR_Name, 11 );
                    name[11] = '\0';
                    printf("%s is in cluster low %d\n", name, dir[i].DIR_FirstClusterLow );
                }
            }
            else
            {
                printf("Error: FIle system image must be opened first.\n");
            }
        }
        ////////  read  ////////
        // Reads from the given file at the position, in bytes, specified byt position parameter
        // Ouput the number of bytes specified.
        else if(strcmp(token[0], "read")==0)
        {
            if(status == 1)
            {
                printf("read working.\n");
            }
            else
            {
                printf("Error: FIle system image must be opened first.\n");
            }
        }
        ////////  volume  ////////
        // Prints the volume name of the file system image.If there is volume name, its found in
        // reserved section. If no volume name, output error.
        else if(strcmp(token[0], "volume")==0)
        {
            if(status == 1)
            {
                printf("volume working.\n");
            }
            else
            {
                printf("Error: FIle system image must be opened first.\n");
            }
        }

        free(working_root);
    }
    return 0;
}
