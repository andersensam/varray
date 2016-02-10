/*
 * Author: Samuel Andersen
 * Date: 2016-02-05
 * Description: Library for emulating C++ style vectors
 * in C
 *
 */

#include "varray.h"

void varrayDemo() {
	varray sampleInt;
	/*
	 * Functions as a normal array but also stores
	 * the size of the array at sampleInt.size.
	 * Accessing data elements used the syntax
	 * VARRAY.getInt[POSITION]
	 */
	varray* sampleString;
	/*
	 * Since a C string is an array of char, creating
	 * an array of varray objects can easily store multiple
	 * strings. The size of the varray array can be accessed
	 * via VARRAY[0].size Individual elements can be 
	 * accessed at VARRAY[POSITION].str with position >= 1
	 */
	varray** sampleContainer;
	/*
	 * The container of varrays can store multiple data types
	 * at each element's position. For example, the syntax
	 * VARRAY[1][1].str will return the string, if stored.
	 * while VARRAY[2][2].getDbl[POSITION] returns the double
	 */
	sampleInt = *(varray*)createArray(v_int, 5);
	/*
	 * The function createArray(TYPE, SIZE) initializes the array
	 * NOTE: I am not sure if there is a more "aesthetically pleasing"
	 * way to initialize the array, since createArray returns a void
	 * pointer. When initialized, all values are set to 0
	 */
	printf("The size of sampleInt is: %zu\n", sampleInt.size);
	printf("The data at position 0 is: %d\n", sampleInt.getInt[0]);

	sampleString = createArray(v_varray, 2);
	/*
	 * Each varray can contain one string. Initialize each element
	 * with the createArray command.
	 */
	printf("The size of sampleString is: %zu\n", sampleString[0].size);
	/*
	 * As noted above, the size is stored in a varray at position 0
	 * in the container
	 */
	sampleString[1] = varrayPush("This is a sample string");
	sampleString[2] = varrayPush("This is also a sample string!"); 
	/*
	 * To store a string within a varray, the function varrayPush is used
	 * with the desired string as the argument. The function initializes
	 * another varray object within the container.
	 */
	printf("The string at position 1 is: %s\n", sampleString[1].str);
	printf("The size of the string stored at position 1 is: %zu\n", sampleString[1].size); 
	printf("The char at position 5 in sampleString[1] is: %c\n", sampleString[1].str[5]);

	sampleContainer = createArray(v_tdvarray, 2);
	sampleContainer[1] = createArray(v_varray, 1);
	sampleContainer[1][1] = *(varray*)createArray(v_double, 5);
	sampleContainer[1][1].getDbl[4] = 3.14;
	sampleContainer[2] = createArray(v_varray, 1);
	sampleContainer[2][1] = varrayPush("yet another sample string");
	/*
	 * As noted with the original sampleInt example, the *(varray*)
	 * syntax is used again.
	 */
	printf("sampleContainer[1][1] has size: %zu with data %lf at position 4\n", sampleContainer[1][1].size, sampleContainer[1][1].getDbl[4]);
	printf("sampleContainer[2][1] has size %zu with string \"%s\"\n", sampleContainer[2][1].size, sampleContainer[2][1].str); 
}

int main() {
	varrayDemo();
	return 0;
}

