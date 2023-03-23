//
//  types.h
//  D3Parser
//
//  Created by Artem Shimanski on 20.09.12.
//  Copyright (c) 2012 Artem Shimanski. All rights reserved.
//

#ifndef D3Parser_types_h
#define D3Parser_types_h

//#include <stdint.h>
typedef int int32_t;
typedef long long int64_t;
typedef int32_t Hash;
typedef unsigned char Byte;

const int DataArrayEntrySize = 37;

typedef struct {
	int32_t mpqMagicNumber;
	int32_t fileTypeId;
	int32_t unused[2];
} MPQHeader;

typedef struct {
	int32_t stlFileId;	// Stl file Id
	int32_t unknown1[5];	// always 0x00000000
	int32_t headerSize;	// size (in bytes) of the StlHeader? (always 0x00000028)
	int32_t entriesSize;	// size (in bytes) of the StlEntries
	int32_t unknown2[2];	// always 0x00000000
} MPQStlHeader;

typedef struct {
	int32_t unknown1[2];	// always 0x00000000
	int32_t string1offset;	// file offset for string1 (non-NLS key)
	int32_t string1size;	// size of string1
	int32_t unknown2[2];	// always 0x00000000
	int32_t string2offset;	// file offset for string2
	int32_t string2size;	// size of string2
	int32_t unknown3[2];	// always 0x00000000
} MPQStlEntry;

typedef struct {
	int32_t mpqMagicNumber;
	int32_t fileTypeId;
	int32_t unused;
	int32_t unknown0;
} D4StlCompatHeader;

typedef struct {
	int32_t stlFileId;	// Stl file Id
	int32_t unknown1[3];	// always 0x00000000
	int32_t headerSize;	// size (in bytes) of the StlHeader? (always 0x00000028)
	int32_t entriesSize;	// size (in bytes) of the StlEntries
	int32_t unknown2[2];	// always 0x00000000
} D4StlHeader;

typedef struct {
	int32_t unknown1[2];	// always 0x00000000
	int32_t string1offset;	// file offset for string1 (non-NLS key)
	int32_t string1size;	// size of string1
	int32_t unknown2[2];	// always 0x00000000
	int32_t string2offset;	// file offset for string2
	int32_t string2size;	// size of string2
	int32_t unknown3[2];	// always 0x00000000
} D4StlEntry;


#endif
