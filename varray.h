/*
 *
 * Author: Samuel Andersen with contribution from vnp (http://codereview.stackexchange.com/users/40480/vnp)
 * Date: 2016-02-16
 * Description: Library for emulating C++ style vectors
 * in C
 *
 */

#ifndef VARRAY_H
#define VARRAY_H
#define VARRAY_DISPLAYDEBUG 0
#define VARRAY_PUSHDEBUG 0
#define VARRAY_CLEARDEBUG 0
#define VARRAY_STRSIZE(x) ((sizeof(char) * strlen(x)) + 1)
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

typedef enum {
	v_int,
	v_double,
	v_char,
	v_varray,
	v_tdvarray,
	v_ref,
	v_tdvref,
	v_str
} varrayType;

typedef struct varray {
	void* data;
	size_t size;
	size_t nextBlock;
	void* (*get)(const struct varray* target, const size_t element);
	void* str;
	void (*push)();
	void (*clear)(void* target);
	varrayType type;
} varray;

void* createArray(varrayType type, size_t size);
varray* varrayPush(const char* data);
void allocateNumArray(varray* target, varrayType type, size_t size);
void allocateCharArray(varray* target, size_t size);
void* vGetInt(const varray* target, const size_t element);
void* vGetDbl(const varray* target, const size_t element);
void* vGetChar(const varray* target, const size_t element);
void vIntPush(varray* target, int pushData);
void vDblPush(varray* target, double pushData);
void vCharPush(varray* target, char* pushData);
void vClear(void* target);
void varrayClear(void* target);
void tdVarrayClear(void* target);

inline void* createArray(varrayType type, size_t size) {
	size_t i;
	void* target;
	if (type == v_int || type == v_double) {
		target = malloc(sizeof(varray));
		allocateNumArray((varray*)target, type, size);
		((varray*)target)->size = size;
		((varray*)target)->nextBlock = 0;
		((varray*)target)->clear = vClear;
	}
	else if (type == v_char) {
		target = malloc(sizeof(varray));
		allocateCharArray((varray*)target, size);
		((varray*)target)->size = size;
		((varray*)target)->nextBlock = 0;
		((varray*)target)->clear = vClear;
		((varray*)target)->type = v_char;
	}
	else if (type == v_varray || type == v_str) {
		target = malloc(sizeof(varray*) * (size + 1));
		((varray**)target)[0] = createArray(v_ref, size);
		for (i = 1; i <= size; i++) {
			((varray**)target)[i] = NULL;
		}
		if (type == v_str) {
			((varray**)target)[0]->type = v_str;
		}
		else {
			((varray**)target)[0]->type = v_varray;
		}
	}
	else if (type == v_tdvarray) {
		target = malloc(sizeof(varray**) * (size + 1));
		((varray***)target)[0] = createArray(v_tdvref, size);
		for (i = 1; i <= size; i++) {
			((varray***)target)[i] = NULL;
		}
		((varray***)target)[0][0]->clear = tdVarrayClear;
		((varray***)target)[0][0]->type = v_tdvarray;
	}
	else if (type == v_ref) {
		target = malloc(sizeof(varray));
		((varray*)target)->size = size;
		((varray*)target)->clear = varrayClear;
		((varray*)target)->data = NULL;
	}
	else if (type == v_tdvref) {
		target = malloc(sizeof(varray*));
		((varray**)target)[0] = createArray(v_ref, size);
		((varray**)target)[0]->clear = tdVarrayClear;
	}
	else {
		return NULL;
	}
	return target;
}

inline varray* varrayPush(const char* data) {
	varray* target = createArray(v_ref, 1);
	if (VARRAY_DISPLAYDEBUG) {
		printf("%s%zu%s%s\n", "Allocating array with size: ", VARRAY_STRSIZE(data) - 1, " with contents: ", data);
	}
	target->data = malloc(VARRAY_STRSIZE(data));
	strcpy(target->data, data);
	if (VARRAY_DISPLAYDEBUG) {
		printf("%s%s\n", "String created successfully. Contents reads: ", (char*)target->data);
		printf("%s%d\n", "Memory address: ", (int)target->data);
	}
	target->size = VARRAY_STRSIZE(data);
	target->str = target->data;
	target->clear = vClear;
	target->type = v_str;
	return target;
}

inline void allocateNumArray(varray* target, varrayType type, size_t size) {
	if (type == v_int) {
		target->data = calloc(size, sizeof(int*));
		target->get = vGetInt;
		target->push = vIntPush;
		target->type = v_int;
	}
	else {
		target->data = calloc(size, sizeof(double*));
		target->get = vGetDbl;
		target->push = vDblPush;
		target->type = v_double;
	}
}

inline void allocateCharArray(varray* target, size_t size) {
	target->data = calloc(size, sizeof(char*));
	target->get = vGetChar;
	target->push = vCharPush;
}


inline void* vGetInt(const varray* target, const size_t element) {
	if (element >= target->size) {
		return target->data;
	}
	else {
		return (int*)target->data + element;
	}
}

inline void* vGetDbl(const varray* target, const size_t element) {
	if (element >= target->size) {
		return target->data;
	}
	else {
		return (double*)target->data + element;
	}
}

inline void* vGetChar(const varray* target, const size_t element) {
	if (element >= target->size) {
		return target->data;
	}
	else {
		return (char*)target->data + element;
	}
}

inline void vIntPush(varray* target, int pushData) {
	int* pInt;
	size_t i;
	void* pData;
	pInt = &pushData;
	if (target->nextBlock == target->size) {
		if (VARRAY_PUSHDEBUG) {
			printf("Allocating pData with size %zu\n", target->size + 1);
		}
		pData = calloc(target->size, sizeof(int*));
		for (i = 0; i < target->size; i++) {
			if (VARRAY_PUSHDEBUG) {
				printf("Copying data for record %zu\n", i);
			}
			((int*)pData)[i] = *(int*)target->get(target, i);
		}
		if (VARRAY_PUSHDEBUG) {
			printf("Attempting to resize target->data with new size: %zu\n", target->size + 1);
		}
		target->data = realloc(target->data, ((target->size + 1) * sizeof(int*)));
		if (target->data == NULL) {
			if (VARRAY_PUSHDEBUG) {
				printf("Failed; realloc was unable to resize the varray, completing manually");
			}
			target->data = calloc(target->size + 1, sizeof(int*));
			for (i = 0; i < target->size; i++) {
				((int*)target->data)[i] = ((int*)pData)[i];
			}
		}
		if (VARRAY_PUSHDEBUG) {
			printf("Adding new record at position %zu\n", target->size);
		}
		((int*)target->data)[target->size] = *pInt;
		if (VARRAY_PUSHDEBUG) {
			printf("Increasing size, nextBlock record, and cleaning up pData\n");
		}
		target->size++;
		target->nextBlock++;
		free(pData);
	}
	else {
		if (VARRAY_PUSHDEBUG) {
			printf("Adding new record at position %zu\n", target->nextBlock);
		}
		((int*)target->data)[target->nextBlock] = *pInt;
		if (VARRAY_PUSHDEBUG) {
			printf("Increasing nextBlock record\n");
		}
		target->nextBlock++;
	}
}

inline void vDblPush(varray* target, double pushData) {
	double pDbl;
	size_t i;
	void* pData;
	pDbl = pushData;
	if (target->nextBlock == target->size) {
		if (VARRAY_PUSHDEBUG) {
			printf("Allocating pData with size %zu\n", target->size + 1);
		}
		pData = calloc(target->size, sizeof(double*));
		for (i = 0; i < target->size; i++) {
			if (VARRAY_PUSHDEBUG) {
				printf("Copying data for record %zu\n", i);
			}
			((double*)pData)[i] = *(double*)target->get(target, i);
		}
		if (VARRAY_PUSHDEBUG) {
			printf("Attempting to resize target->data with new size: %zu\n", target->size + 1);
		}
		target->data = realloc(target->data, ((target->size + 1) * sizeof(double*)));
		if (target->data == NULL) {
			if (VARRAY_PUSHDEBUG) {
				printf("Failed; realloc was unable to resize the varray, completing manually");
			}
			target->data = calloc(target->size + 1, sizeof(double*));
			for (i = 0; i < target->size; i++) {
				((double*)target->data)[i] = ((double*)pData)[i];
			}
		}
		if (VARRAY_PUSHDEBUG) {
			printf("Adding new record at position %zu\n", target->size);
		}
		((double*)target->data)[target->size] = pDbl;
		if (VARRAY_PUSHDEBUG) {
			printf("Increasing size, nextBlock record, and cleaning up pData\n");
		}
		target->size++;
		target->nextBlock++;
		free(pData);
	}
	else {
		if (VARRAY_PUSHDEBUG) {
			printf("Adding new record at position %zu\n", target->nextBlock);
		}
		((double*)target->data)[target->nextBlock] = pDbl;
		if (VARRAY_PUSHDEBUG) {
			printf("Increasing nextBlock record\n");
		}
		target->nextBlock++;
	}
}

inline void vCharPush(varray* target, char* pushData) {
	char pChar;
	size_t i;
	void* pData;
	pChar = *pushData;
	if (target->nextBlock == target->size) {
		if (VARRAY_PUSHDEBUG) {
			printf("Allocating pData with size %zu\n", target->size + 1);
		}
		pData = calloc(target->size, sizeof(char*));
		for (i = 0; i < target->size; i++) {
			if (VARRAY_PUSHDEBUG) {
				printf("Copying data for record %zu\n", i);
			}
			((char*)pData)[i] = *(char*)target->get(target, i);
		}
		if (VARRAY_PUSHDEBUG) {
			printf("Attempting to resize target->data with new size: %zu\n", target->size + 1);
		}
		target->data = realloc(target->data, ((target->size + 1) * sizeof(char*)));
		if (target->data == NULL) {
			if (VARRAY_PUSHDEBUG) {
				printf("Failed; realloc was unable to resize the varray, completing manually");
			}
			target->data = calloc(target->size + 1, sizeof(char*));
			for (i = 0; i < target->size; i++) {
				((char*)target->data)[i] = ((char*)pData)[i];
			}
		}
		if (VARRAY_PUSHDEBUG) {
			printf("Adding new record at position %zu\n", target->size);
		}
		((char*)target->data)[target->size] = pChar;
		if (VARRAY_PUSHDEBUG) {
			printf("Increasing size, nextBlock record, and cleaning up pData\n");
		}
		target->size++;
		target->nextBlock++;
		free(pData);
	}
	else {
		if (VARRAY_PUSHDEBUG) {
			printf("Adding new record at position %zu\n", target->nextBlock);
		}
		((char*)target->data)[target->nextBlock] = pChar;
		if (VARRAY_PUSHDEBUG) {
			printf("Increasing nextBlock record\n");
		}
		target->nextBlock++;
	}
}

inline void vClear(void* target) {
	varray* pTarget = target;
	if (VARRAY_CLEARDEBUG) {
		printf("Launching vClear\n");
	}
	free(pTarget->data);
	free(target);
}

inline void varrayClear(void* target) {
	varray** pTarget = target;
	size_t i;
	if (VARRAY_CLEARDEBUG) {
		printf("Launching varrayClear\n");
	}
	for (i = 1; i <= pTarget[0]->size; i++) {	
		if (VARRAY_CLEARDEBUG) {
			printf("Clearing varray at address: %d with size: %zu\n", (int)pTarget[i], sizeof(pTarget[i]));
		}
		if (pTarget[i] == NULL) {
			continue;
		}
		else {
			pTarget[i]->clear(pTarget[i]);
		}
	}
	free(pTarget[0]);
	free(target);
}

inline void tdVarrayClear(void* target) {
	varray*** pTarget = target;
	size_t i, j;
	if (VARRAY_CLEARDEBUG) {
		printf("Launching tdVarrayClear\n");
		printf("Reading size of pTarget[0][0]\n");
	}
	for (i = 1; i <= pTarget[0][0]->size; i++) {
		if (VARRAY_CLEARDEBUG) {
			printf("Reading size of pTarget[%zu][0]\n", i);
		}
		for (j = 1; j <= pTarget[i][0]->size; j++) {
			if (pTarget[i][j] == NULL) {
				continue;
			}
			else {
				if (VARRAY_CLEARDEBUG) {
					printf("Clearing pTarget[%zu][%zu]\n", i, j);
				}
				pTarget[i][j]->clear(pTarget[i][j]);
			}
		}
	}
	if (VARRAY_CLEARDEBUG) {
		printf("Clearing pTarget[0][0]\n");
	}
	free(pTarget[0][0]);
	if (VARRAY_CLEARDEBUG) {
		printf("Clearing pTarget [0]\n");
	}
	free(pTarget[0]);
	if (VARRAY_CLEARDEBUG) {
		printf("Clearing target\n");
	}
	free(target);
}

#endif
