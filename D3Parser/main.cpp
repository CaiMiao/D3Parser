//
//  main.cpp
//  D3Parser
//
//  Created by Artem Shimanski on 19.09.12.
//  Copyright (c) 2012 Artem Shimanski. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <stdio.h>

#include "types.h"
#include <map>
#include <string>
#include <vector>
#include <list>

#ifdef WIN32
#include <io.h>
#else
#include <dirent.h>
#endif

#include <unordered_set>
#include "tinyxml.h"

class Range {
public:
	Range operator+(Range& other) {
		Range result = *this;
		result.min += other.min;
		result.max += other.max;
		return result;
	}
	
	Range operator-(Range& other) {
		Range result = *this;
		result.min -= other.min;
		result.max -= other.max;
		return result;
	}
	
	Range operator*(Range& other) {
		Range result = *this;
		result.min *= other.min;
		result.max *= other.max;
		return result;
	}
	
	Range operator/(Range& other) {
		Range result = *this;
		result.min /= other.min;
		result.max /= other.max;
		return result;
	}
	
	float min;
	float max;
	
};

typedef int FileID;
std::map<FileID, std::string> filesMap;

typedef std::map<Hash, std::vector<std::string> > StringsHashMap;
//std::map<Hash, std::vector<std::string> > stringsHashMap;
std::map<int, StringsHashMap > stringsHashMap;

Hash hash(const char* string) {
	Hash hash = 0;
	size_t n = strlen(string);
	for (size_t i = 0; i < n; i++)
		hash = (hash * 0x21) + tolower(string[i]);
	return hash;
}

bool isValidHash(Hash h) {
	return h != 0 && h != -1;
}

Byte* fileContents(const char* fileName, size_t* fileSize) {
	FILE* f = fopen(fileName, "rb");
	if (!f)
		return NULL;
	
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (size == 0)
		return NULL;
	
	Byte* memory = new Byte[size];
	fread(memory, size, 1, f);
	fclose(f);
	
	if (fileSize)
		*fileSize = size;
	return memory;
}

void processStlFile(D4StlHeader* stlHeader) {
	int count = stlHeader->entriesSize / sizeof(D4StlEntry);
	D4StlEntry* pStlEntry = reinterpret_cast<D4StlEntry*>(stlHeader + 1);
	for (int i = 0; i < count; i++) {
		if (i == 48) {
			i = i;
		}
		std::vector<std::string> strings;
		strings.reserve(4);
		int32_t offsets[] = {pStlEntry->string1offset, pStlEntry->string2offset};
		int32_t sizes[] = {pStlEntry->string1size, pStlEntry->string2size};
		std::string key;
		for (int i = 0; i < 2; i++) {
			if (offsets[i] > 0 && sizes[i] > 1) {
				const char* s = reinterpret_cast<char*>(stlHeader) + offsets[i];
				strings.push_back(s);
				if (key.length() == 0)
					key = s;
			}
			else
				strings.push_back("");
		}
		Hash h = hash(key.c_str());
		stringsHashMap[stlHeader->stlFileId][h] = strings;
		pStlEntry += 1;
	}
}

void processFile(const std::string& fileName) {
	size_t size = 0;
	Byte* contents = fileContents(fileName.c_str(), &size);
	if (!contents)
		return;
	
	D4StlCompatHeader* mpqHeader = (D4StlCompatHeader*) contents;
	std::string extension = fileName.substr(fileName.find_last_of(".") + 1);
	if (extension == "stl") {
		D4StlHeader* stlHeader = reinterpret_cast<D4StlHeader*>(mpqHeader + 1);
		processStlFile(stlHeader);
		filesMap[stlHeader->stlFileId] = fileName;
	}
	delete[] contents;
}

std::vector<std::string> stlFiles() {
	std::string dirPath = "StringList/";
	
#ifdef WIN32
	struct _finddata_t c_file;
	intptr_t hFile;

	std::vector<std::string> files;
	if( (hFile = _findfirst( "./StringList/*.stl", &c_file )) != -1) {
		do {
			std::string fileName = c_file.name;
			files.push_back(dirPath + fileName);
		} while( _findnext( hFile, &c_file ) == 0 );
		_findclose( hFile );
	}

#else
	DIR* dir = opendir(dirPath.c_str());
	std::vector<std::string> files;
	struct dirent* ent;
	if (dir) {
		while ((ent = readdir(dir)) != NULL) {
			std::string fileName = ent->d_name;
			std::string extension = fileName.substr(fileName.find_last_of(".") + 1);
			if (extension == "stl")
				files.push_back(dirPath + fileName);
		}
	}
#endif
	return files;
}

/*std::string stringRepresentation(const ModCodeData& memoryBlock)
{
	size_t len = 3 + memoryBlock.size() * 2;
	char *buf = new char[len + 1];
	int i = 2;
	//const Byte *ptr = memoryBlock.c_str();
	//const Byte *end = ptr + memoryBlock.size();
	buf[0] = 'X';
	buf[1] = buf[len - 1] = '\'';
	buf[len] = 0;
	
	const char *map= "0123456789ABCDEF";
	
//	for (; ptr != end; ptr++)
	for (Byte byte: memoryBlock)
	{
		Byte l = byte & 0xf;
		Byte h = byte >> 4;
		buf[i++] = map[h];
		buf[i++] = map[l];
	}
	
	std::string s(buf);
	delete[] buf;
	return s;
}*/

std::string replaceQuotes(const std::string& s) {
	std::string ss;
	for (auto c: s) {
		if (c == '"')
			ss += '"';
		ss += c;
	}
	return ss;
}

void processStrResults() {
	std::ofstream stringsCSV("strings.csv", std::ios::out | std::ios::trunc);

	for (auto i : stringsHashMap) {
		const auto resourceFile = filesMap[i.first].c_str();
		printf_s("processing %s \n", resourceFile);
		for (auto j : i.second) {
			const std::vector<std::string>& strings = j.second;
			stringsCSV << "\"" << resourceFile <<
				"\",\"" << replaceQuotes(strings[0]) <<
				"\",\"" << replaceQuotes(strings[1]) << "\"" << std::endl;
		}
	}
}

int main(int argc, const char * argv[])
{
	std::vector<std::string> stls = stlFiles();
	for (auto path: stls) {
		processFile(path.c_str());
	}

	processStrResults();
}

