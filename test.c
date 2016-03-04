/*
 * Varray Demo
 * Version: 2016-02-28
 * Author: Sam Andersen
 * Email: samstext@gmail.com
 *
 * General Notes:
 *
 */

#include <stdlib.h>
#include <stdio.h> 
#include "varray.h"

void basicTest() {
	size_t i;
	/* Initialize the array */
	varray* intArray = createArray(v_int, 2);
	/* Set sample values */
	intArray->push(intArray, 123);
	intArray->push(intArray, 456);
	/* Read back the values */
	printf("Size of intArray: %zu\n\n", intArray->size);
	for (i = 0; i < intArray->size; i++) {
		printf("Value at position %zu: %d\n", i, *(int*)intArray->get(intArray, i));
	}
	/* Testing push feature to add more data */
	intArray->push(intArray, 789);
	intArray->push(intArray, 43110);
	/* Verifying information */
	printf("\nNew size of intArray: %zu\n\n", intArray->size);
	/* Verify data */
	for (i = 0; i < intArray->size; i++) {
		printf("Value at position %zu: %d\n", i, *(int*)intArray->get(intArray, i));
	}
	/* Cleanup the array */
	intArray->clear(intArray);
} 

void stringTest() {
	size_t i;
	/* Initialize the array */
	varray** stringArray = createArray(v_str, 2);
	/* Set sample values */
	stringArray[1] = varrayPush("This is a test string");
	stringArray[2] = varrayPush("This is another test string!");
	/* Read back the values */
	printf("\nSize of stringArray: %zu\n\n", stringArray[0]->size);
	for (i = 1; i <= stringArray[0]->size; i++) {
		printf("Value at position %zu: %s\n", i, stringArray[i]->str);
	}
	stringArray[0]->clear(stringArray);
}

void varrayTest() {

}

void tdvarrayTest() {

}

int main(void) {
	basicTest();
	stringTest();
	return 0;
}
