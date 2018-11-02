/*
  Name:             ID:
  Carlos Meza       1001170229
  Matt Crum         ** Matt, add your id number**
*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>

FILE *fp;

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

int main()
{
    fp = fopen( "fat32.img", "r");

    if( fp == NULL )
    {
        perror("Error opening file: ");
    }

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

    // Calculating the address of the root directory
    RootClusAddress = (BPB_NumFATs * BPB_FATz32 * BPB_BytesPerSec) + (BPB_RsvdSecCnt * BPB_BytesPerSec);

    // Print statement to check if everything initialized correctly
    printf("RootClusterAddress %x\n", RootClusAddress );

    fseek( fp, RootClusAddress, SEEK_SET );


    // Can get entry to any directory sector, can be added to LBAToOffset function
    // to return address for that sectords
    int i;
    for( i = 0; i < 16; i++ )
    {
        fread( &dir[i], sizeof( struct DirectoryEntry ), 1, fp );
    }

    // printing how clusters are layed out
    for(i = 0; i < 16; i++)
    {
        char name[12];
        memcpy( name, dir[i].DIR_Name, 11 );
        name[11] = '\0';
        printf("%s is in cluster low %d\n", name, dir[i].DIR_FirstClusterLow );
    }

    return 0;
}