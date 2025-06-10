#pragma once
#include <ntdef.h>

typedef unsigned long       DWORD;
typedef unsigned short      WORD;

typedef struct _IMAGE_SECTION_HEADER
{
    UCHAR Name[8];                                                          //0x0
    union
    {
        ULONG PhysicalAddress;                                              //0x8
        ULONG VirtualSize;                                                  //0x8
    } Misc;                                                                 //0x8
    ULONG VirtualAddress;                                                   //0xc
    ULONG SizeOfRawData;                                                    //0x10
    ULONG PointerToRawData;                                                 //0x14
    ULONG PointerToRelocations;                                             //0x18
    ULONG PointerToLinenumbers;                                             //0x1c
    USHORT NumberOfRelocations;                                             //0x20
    USHORT NumberOfLinenumbers;                                             //0x22
    ULONG Characteristics;                                                  //0x24
}IMAGE_SECTION_HEADER, * PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY
{
    ULONG VirtualAddress;                                                   //0x0
    ULONG Size;                                                             //0x4
}IMAGE_DATA_DIRECTORY, * PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_FILE_HEADER
{
    USHORT Machine;                                                         //0x0
    USHORT NumberOfSections;                                                //0x2
    ULONG TimeDateStamp;                                                    //0x4
    ULONG PointerToSymbolTable;                                             //0x8
    ULONG NumberOfSymbols;                                                  //0xc
    USHORT SizeOfOptionalHeader;                                            //0x10
    USHORT Characteristics;                                                 //0x12
}IMAGE_FILE_HEADER, * PIMAGE_FILE_HEADER;

typedef struct _IMAGE_OPTIONAL_HEADER64
{
    USHORT Magic;                                                           //0x0
    UCHAR MajorLinkerVersion;                                               //0x2
    UCHAR MinorLinkerVersion;                                               //0x3
    ULONG SizeOfCode;                                                       //0x4
    ULONG SizeOfInitializedData;                                            //0x8
    ULONG SizeOfUninitializedData;                                          //0xc
    ULONG AddressOfEntryPoint;                                              //0x10
    ULONG BaseOfCode;                                                       //0x14
    ULONGLONG ImageBase;                                                    //0x18
    ULONG SectionAlignment;                                                 //0x20
    ULONG FileAlignment;                                                    //0x24
    USHORT MajorOperatingSystemVersion;                                     //0x28
    USHORT MinorOperatingSystemVersion;                                     //0x2a
    USHORT MajorImageVersion;                                               //0x2c
    USHORT MinorImageVersion;                                               //0x2e
    USHORT MajorSubsystemVersion;                                           //0x30
    USHORT MinorSubsystemVersion;                                           //0x32
    ULONG Win32VersionValue;                                                //0x34
    ULONG SizeOfImage;                                                      //0x38
    ULONG SizeOfHeaders;                                                    //0x3c
    ULONG CheckSum;                                                         //0x40
    USHORT Subsystem;                                                       //0x44
    USHORT DllCharacteristics;                                              //0x46
    ULONGLONG SizeOfStackReserve;                                           //0x48
    ULONGLONG SizeOfStackCommit;                                            //0x50
    ULONGLONG SizeOfHeapReserve;                                            //0x58
    ULONGLONG SizeOfHeapCommit;                                             //0x60
    ULONG LoaderFlags;                                                      //0x68
    ULONG NumberOfRvaAndSizes;                                              //0x6c
    IMAGE_DATA_DIRECTORY DataDirectory[16];                         //0x70
}IMAGE_OPTIONAL_HEADER, * PIMAGE_OPTIONAL_HEADER;

typedef struct _IMAGE_NT_HEADERS64
{
    ULONG Signature;                                                        //0x0
    IMAGE_FILE_HEADER FileHeader;                                   //0x4
    IMAGE_OPTIONAL_HEADER OptionalHeader;                         //0x18
}IMAGE_NT_HEADERS, * PIMAGE_NT_HEADERS;

typedef struct _IMAGE_DOS_HEADER
{
    USHORT e_magic;                                                         //0x0
    USHORT e_cblp;                                                          //0x2
    USHORT e_cp;                                                            //0x4
    USHORT e_crlc;                                                          //0x6
    USHORT e_cparhdr;                                                       //0x8
    USHORT e_minalloc;                                                      //0xa
    USHORT e_maxalloc;                                                      //0xc
    USHORT e_ss;                                                            //0xe
    USHORT e_sp;                                                            //0x10
    USHORT e_csum;                                                          //0x12
    USHORT e_ip;                                                            //0x14
    USHORT e_cs;                                                            //0x16
    USHORT e_lfarlc;                                                        //0x18
    USHORT e_ovno;                                                          //0x1a
    USHORT e_res[4];                                                        //0x1c
    USHORT e_oemid;                                                         //0x24
    USHORT e_oeminfo;                                                       //0x26
    USHORT e_res2[10];                                                      //0x28
    LONG e_lfanew;                                                          //0x3c
}IMAGE_DOS_HEADER, * PIMAGE_DOS_HEADER;

typedef struct _IMAGE_IMPORT_DESCRIPTOR
{
    union
    {
        DWORD   Characteristics;
        DWORD   OriginalFirstThunk;
    } DUMMYUNIONNAME;
    DWORD   TimeDateStamp;
    DWORD   ForwarderChain;
    DWORD   Name;
    DWORD   FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR, * PIMAGE_IMPORT_DESCRIPTOR;

typedef struct _IMAGE_IMPORT_BY_NAME
{
    WORD    Hint;
    CHAR   Name[1];
} IMAGE_IMPORT_BY_NAME, * PIMAGE_IMPORT_BY_NAME;

typedef struct _IMAGE_THUNK_DATA
{
    union {
        ULONGLONG ForwarderString;  // PBYTE 
        ULONGLONG Function;         // PDWORD
        ULONGLONG Ordinal;
        ULONGLONG AddressOfData;    // PIMAGE_IMPORT_BY_NAME
    } u1;
} IMAGE_THUNK_DATA, * PIMAGE_THUNK_DATA;

typedef struct _IMAGE_EXPORT_DIRECTORY
{
    DWORD   Characteristics;
    DWORD   TimeDateStamp;
    WORD    MajorVersion;
    WORD    MinorVersion;
    DWORD   Name;
    DWORD   Base;
    DWORD   NumberOfFunctions;
    DWORD   NumberOfNames;
    DWORD   AddressOfFunctions;     // RVA from base of image
    DWORD   AddressOfNames;         // RVA from base of image
    DWORD   AddressOfNameOrdinals;  // RVA from base of image
} IMAGE_EXPORT_DIRECTORY, * PIMAGE_EXPORT_DIRECTORY;

typedef struct _IMAGE_BASE_RELOCATION {
    DWORD   VirtualAddress;
    DWORD   SizeOfBlock;
}IMAGE_BASE_RELOCATION;
typedef IMAGE_BASE_RELOCATION UNALIGNED* PIMAGE_BASE_RELOCATION;