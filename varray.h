/*
 *
 * Author: Samuel Andersen with contribution from vnp (http://codereview.stackexchange.com/users/40480/vnp)
 * Date: 2016-02-05
 * Description: Library for emulating C++ style vectors
 * in C
 *
 */

#ifndef VARRAY_H
#define VARRAY_H
#define VARRAY_DISPLAYDEBUG 0
#define VARRAY_STRSIZE(x) ((sizeof(char) * strlen(x)) + 1)
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

typedef struct {
	int* getInt;
	char* getChar;
	double* getDbl;
	char* str;
	size_t size;
} varray;

typedef enum {
	v_char = 0,
	v_int = 1,
	v_double = 2,
	v_varray = 3,
	v_tdvarray = 4
} varrayType;

void* createArray(varrayType arrayType, size_t arraySize);
varray varrayPush(const char* data);
varray allocateNumArray(varrayType type, const size_t size);
varray allocateCharArray(const size_t size);
varray* allocateVarray(const size_t size);
varray** allocateTDVarray(const size_t size);

inline void* createArray(varrayType arrayType, size_t arraySize) {
	varray* target;
	varray** vtarget;
	varray*** tdvtarget;
	if (arrayType == v_char) {
		target = malloc(sizeof(varray));
		*target = allocateCharArray(arraySize);
		return target;
	}
	else if (arrayType == v_int || arrayType == v_double) {
		target = malloc(sizeof(varray));
		*target = allocateNumArray(arrayType, arraySize);
		return target;
	}
	else if (arrayType == v_varray) {
		vtarget = malloc(sizeof(varray*));
		*vtarget = allocateVarray(arraySize);
		return *vtarget;
	}
	else if (arrayType == v_tdvarray) {
		tdvtarget = malloc(sizeof(varray**));
		*tdvtarget = allocateTDVarray(arraySize);
		return *tdvtarget;
	}
	else {
		return NULL;
	}
}	

inline varray varrayPush(const char* data) {
	varray target;
	if (VARRAY_DISPLAYDEBUG) {
		printf("%s%zu%s%s\n", "Allocating array with size: ", VARRAY_STRSIZE(data) - 1, " with contents: ", data);
	}
	target.str = strdup(data);
	if (VARRAY_DISPLAYDEBUG) {
		printf("%s%s\n", "String created successfully. Contents reads: ", target.str);
		printf("%s%d\n", "Memory address: ", (int)target.str);
	}
	target.size = VARRAY_STRSIZE(data);
	return target;
}

inline varray allocateNumArray(varrayType type, const size_t size) {
	varray target;
	if (type == v_int) {
		if (VARRAY_DISPLAYDEBUG) {
			printf("%s%zu\n", "Allocating array of type v_int with size ", size);
		}
		target.getInt = calloc(size, sizeof(int));
	}
	else if (type == v_double) {
		if (VARRAY_DISPLAYDEBUG) {
			printf("%s%zu\n", "Allocating array of type v_double with size ", size);
		}
		target.getDbl = calloc(size, sizeof(double));
	}
	target.size = size;
	return target;
}

inline varray allocateCharArray(const size_t size) {
	varray target;
	if (VARRAY_DISPLAYDEBUG) {
		printf("%s%zu\n", "Allocating array of type v_char with size ", size);
	}
	target.getChar = calloc(size, sizeof(char));
	target.size = size;
	return target;
}

inline varray* allocateVarray(const size_t size) {
	varray* target = malloc(sizeof(varray) * (size + 1));
	if (VARRAY_DISPLAYDEBUG) {
		printf("%s%zu\n", "Allocated array of type v_varray with size ", size);
	}
	target[0].size = size;
	return target;
}

inline varray** allocateTDVarray(const size_t size) {
	varray** target = malloc(sizeof(varray*) * (size + 1));
	if (VARRAY_DISPLAYDEBUG) {
		printf("%s%zu\n", "Allocated array of type v_tdvarray with size ", size);
	}
	target[0] = malloc(sizeof(varray));
	target[0][0].size = size;
	return target;
}

#endif
