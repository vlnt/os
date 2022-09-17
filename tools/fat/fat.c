#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t bool;
#define true 1
#define false 0

typedef struct{

    uint8_t BootJumpInstruction[3];
    uint8_t OemIdentifier[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FatCount;
    uint16_t DirEntryCount;
    uint16_t TotalSectors;
    uint8_t MediaDescriptorType;
    uint16_t SectorsPerFat;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t LargeSectorsCount;

    //extended boot record
    uint8_t DriveNumber;
    uint8_t Reserved;
    uint8_t Signature;
    uint32_t VolumeId;
    uint8_t VolumeLabel[11];
    uint8_t SystemId[8];
    //bdb_oem:                  db 'MSWIN4.1'        ; 8 bytes
    //bdb_bytes_per_sector:     dw 512
    //bdb_sectors_per_cluster:  db 1
    //bdb_reserved_sectors:     dw 1
    //bdb_fat_count:            db 2
    //bdb_dir_entries_count:    dw 0E0h
    //bdb_total_sectors:        dw 2880              ; 2880 x 512 = 1.44MB
    //bdb_media_descriptor_type:db 0F0h              ; F0 = 3,5" floppy disk
    //bdb_sectors_per_fat:      dw 9                 ; 9 sectors/fat
    //bdb_sectors_per_track:    dw 18
    //bdb_heads:                dw 2
    //bdb_hidden_sectors:       dd 0
    //bdb_large_sectors_count:  dd 0

    // ; extended boot record
    //ebr_drive_number:         db 0                 ; 0x00 - floppy 0x80 - hdd
    //                          db 0                 ; reserved
   // ebr_signature:            db 29h
   // ebr_volume_id:            db 12h, 34h, 56h, 78h   ; serial number
    //ebr_volume_label:         db 'BOGDANA OS '     ; 11 bytes
    //ebr_system_id:            db 'FAT12   '        ; 8bytes
} __attribute__((packed)) BootSector;

typedef struct{
    uint8_t Name[11];
    uint8_t Attributes;
    uint8_t _Reserved;
    uint8_t CreatedTimeTenths;
    uint16_t CreatedTime;
    uint16_t CreatedDate;
    uint16_t AccessedDate;
    uint16_t FirstClusterHigh;
    uint16_t ModifiedTime;
    uint16_t ModifiedDate;
    uint16_t FirstClusterLow;
    uint32_t size;
} __attribute__((packed)) DirectoryEntry;

BootSector g_BootSector;
uint8_t* g_Fat = NULL;
DirectoryEntry* g_RootDirectory = NULL;

bool readBootSector(FILE* disk){
    
    return fread(&g_BootSector, sizeof(BootSector), 1, disk);
}

bool readSectors(FILE* disk, uint32_t lba, uint32_t count, void* bufferOut){

    bool ok = true;
    ok = ok && (fseek(disk, lba * g_BootSector.BytesPerSector, SEEK_SET) == 0);
    ok = ok && (fread(bufferOut, g_BootSector.BytesPerSector, count, disk) == count);
    return ok;
}

bool readFat(FILE* disk){
    
    g_Fat = (uint8_t*)malloc(g_BootSector.SectorsPerFat * g_BootSector.BytesPerSector);
    return readSectors(disk, g_BootSector.ReservedSectors, g_BootSector.SectorsPerFat, g_Fat); 
}

bool readRootDirectory(FILE* disk){

    uint32_t lba = g_BootSector.ReservedSectors * g_BootSector.SectorsPerFat * g_BootSector.FatCount;
    uint32_t size = sizeof(DirectoryEntry) * g_BootSector.DirEntryCount;
    uint32_t sectors = (size / g_BootSector.BytesPerSector);
    if(size % g_BootSector.BytesPerSector > 0){
        sectors++;
    }
    g_RootDirectory = (DirectoryEntry*) malloc(sectors * g_BootSector.BytesPerSector);
    return readSectors(disk, lba, sectors, g_RootDirectory);
}

DirectoryEntry* findFile(const char* name){

    for(uint32_t i = 0; i < g_BootSector.DirEntryCount; i++){

        if(memcmp(name, g_RootDirectory[i].Name, 11) == 0){

            return &g_RootDirectory[i];
        }
    }
    return NULL;
}

int main(int argc, char** argv){

    if(argc < 3){
        printf("Syntax: %s <disk image> <file name>\n", argv[0]);
        return -1;
    }

    FILE* disk = fopen(argv[1], "rb");
    if(!disk){
        fprintf(stderr, "Cannot open disk image %s\n", argv[1]);
        return -1;
    }

    if(!readBootSector(disk)){
        fprintf(stderr, "Could not read boot sector");
        return -2;
    }

    if(!readFat(disk)){
        fprintf(stderr, "Could not read FAT\n");
        free(g_Fat);
        return -3;
    }

    if(!readRootDirectory(disk)){
        fprintf(stderr, "Could not read FAT\n");
        free(g_Fat);
        free(g_RootDirectory);
        return -4;
    }

    DirectoryEntry* fileEntry = findFile(argv[2]);
    if(!fileEntry){
        fprintf(stderr, "Could not find file %s\n", argv[2]);
        free(g_Fat);
        free(g_RootDirectory);
        return -5;
    }

    free(g_Fat);
    free(g_RootDirectory);

    return 0;
}