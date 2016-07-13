/*
 *
 * Author: Samuel Andersen with contribution from vnp (http://codereview.stackexchange.com/users/40480/vnp)
 * Date: 2016-05-15
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
	v_obj
} varrayType;

typedef struct varray {
	void* data;
	size_t size;
	size_t nextBlock;
	char* str;
	varrayType type;

	void* (*get)(const struct varray* target, const size_t element);
	void (*push)();
	void (*clear)(void* target);
} varray;

varray* createArray(varrayType type, size_t size);
varray* varrayPush(const char* data);

void allocateNumArray(varray* target, varrayType type, size_t size);
void allocateObjArray(varray* target, size_t size);
void allocateCharArray(varray* target, size_t size);

void* vGetInt(const varray* target, const size_t element);
void* vGetDbl(const varray* target, const size_t element);
void* vGetObj(const varray* target, const size_t element);
void* vGetChar(const varray* target, const size_t element);

void vIntPush(varray* target, int pushData);
void vDblPush(varray* target, double pushData);
void vObjPush(varray* target, void* pushData);
void vCharPush(varray* target, char* pushData);

void vClear(void* target);
void vObjClear(void* target);

inline varray* createArray(varrayType type, size_t size) {
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

	else if (type == v_obj) {
		target = malloc(sizeof(varray));
		allocateObjArray((varray*)target, size);
		((varray*)target)->size = size;
		((varray*)target)->nextBlock = 0;
		((varray*)target)->clear = vObjClear;
	}

	else {
		return NULL;
	}

	return target;
}

inline varray* varrayPush(const char* data) {
	varray* target = createArray(v_char, VARRAY_STRSIZE(data));

	if (VARRAY_DISPLAYDEBUG) {
		printf("%s%zu%s%s\n", "Allocating array with size: ", VARRAY_STRSIZE(data) - 1, " with contents: ", data);
	}

	strcpy(target->data, data);

	if (VARRAY_DISPLAYDEBUG) {
		printf("%s%s\n", "String created successfully. Contents reads: ", (char*)target->data);
		printf("%s%d\n", "Memory address: ", (int)target->data);
	}

	target->str = target->data;

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

inline void allocateObjArray(varray* target, size_t size) {
	target->data = calloc(size, sizeof(void**));
	target->get = vGetObj;
	target->push = vObjPush;
	target->type = v_obj;
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

inline void* vGetObj(const varray* target, const size_t element) {
	if (element >= target->size) {
		return (*(void**)target->data);
	}
	else {
		return ((void**)target->data)[element];
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
	pInt = &pushData;

	if (target->nextBlock == target->size) {
		
		if (VARRAY_PUSHDEBUG) {
			printf("Attempting to resize target->data with new size: %zu\n", target->size + 1);
		}

		target->data = realloc(target->data, ((target->size + 1) * sizeof(int*)));
		
		if (VARRAY_PUSHDEBUG) {
			printf("Adding new record at position %zu\n", target->size);
		}

		((int*)target->data)[target->size] = *pInt;

		if (VARRAY_PUSHDEBUG) {
			printf("Increasing size, nextBlock record, and cleaning up pData\n");
		}

		target->size++;
		target->nextBlock++;
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
	pDbl = pushData;

	if (target->nextBlock == target->size) {
		
		if (VARRAY_PUSHDEBUG) {
			printf("Attempting to resize target->data with new size: %zu\n", target->size + 1);
		}

		target->data = realloc(target->data, ((target->size + 1) * sizeof(double*)));
		
		if (VARRAY_PUSHDEBUG) {
			printf("Adding new record at position %zu\n", target->size);
		}

		((double*)target->data)[target->size] = pDbl;

		if (VARRAY_PUSHDEBUG) {
			printf("Increasing size, nextBlock record, and cleaning up pData\n");
		}

		target->size++;
		target->nextBlock++;
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

inline void vObjPush(varray* target, void* pushData) {
	void* pObj;
	pObj = pushData;

	if (target->nextBlock == target->size) {
		
		if (VARRAY_PUSHDEBUG) {
			printf("Attempting to resize target->data with new size: %zu\n", target->size + 1);
		}
		target->data = realloc(target->data, ((target->size + 1) * sizeof(void**)));

		if (VARRAY_PUSHDEBUG) {
			printf("Adding new record at position %zu\n", target->size);
		}

		((void**)target->data)[target->size] = pObj;

		if (VARRAY_PUSHDEBUG) {
			printf("Increasing size, nextBlock record, and cleaning up pData\n");
		}

		target->size++;
		target->nextBlock++;
	}
	else {
		if (VARRAY_PUSHDEBUG) {
			printf("Adding new record at position %zu\n", target->nextBlock);
		}

		((void**)target->data)[target->nextBlock] = pObj;

		if (VARRAY_PUSHDEBUG) {
			printf("Increasing nextBlock record\n");
		}

		target->nextBlock++;
	}
}

inline void vCharPush(varray* target, char* pushData) {
	char pChar;
	pChar = *pushData;

	if (target->nextBlock == target->size) {

		if (VARRAY_PUSHDEBUG) {
			printf("Attempting to resize target->data with new size: %zu\n", target->size + 1);
		}

		target->data = realloc(target->data, ((target->size + 1) * sizeof(char*)));

		if (VARRAY_PUSHDEBUG) {
			printf("Adding new record at position %zu\n", target->size);
		}
		
		((char*)target->data)[target->size] = pChar;
		
		if (VARRAY_PUSHDEBUG) {
			printf("Increasing size, nextBlock record, and cleaning up pData\n");
		}

		target->size++;
		target->nextBlock++;
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

inline void vObjClear(void* target) {
	varray* pTarget = target;

	if (VARRAY_CLEARDEBUG) {
		printf("Launching vObjClear\n");
	}

	free(pTarget->data);
	free(target);
}

#define defineArrayType(type) \
	typedef struct type##Array { \
		varray* type##Info; \
		type* (*get)(const struct type##Array* target, size_t element); \
		void (*push)(struct type##Array* target, type* pushData); \
		void (*clear)(struct type##Array* target); \
		size_t size; \
	} type##Array; \
	\
	type* type##ArrayGet(const type##Array* target, size_t element) { \
		return (type*)(target->type##Info->get(target->type##Info, element)); \
	} \
	\
	void type##ArrayPush(type##Array* target, type* pushData) { \
		target->type##Info->push(target->type##Info, pushData); \
		target->size++; \
	} \
	\
	void type##ArrayClear(type##Array* target) { \
		target->type##Info->clear(target->type##Info); \
		free(target); \
	} \
	\
	type##Array* init##type##Array() { \
		type##Array* newArray = malloc(sizeof(type##Array)); \
		\
		newArray->type##Info = createArray(v_obj, 1); \
		newArray->get = type##ArrayGet; \
		newArray->push = type##ArrayPush; \
		newArray->clear = type##ArrayClear; \
		newArray->size = 0;\
		return newArray;\
	}

#endif
