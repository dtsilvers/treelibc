/*
 =============================================================================
 Name        : treelibc_test.c
 Author      : David T. Silvers Sr.
 Contact     : davidtsilvers@aol.com
 Date        : 2014-02-03
 Version     : 1.00
 License     : GNU LGPL
 Description : Tester and usage example for treelibc
               treelibc "Associative Balanced Tree Container"
               SEE: treelibc.c AND treelibc.h

 Copyright   : Copyright 2014 by David T. Silvers Sr.

	This file is part of treelibc.

    treelibc is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    treelibc is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with treelibc. If not, see <http://www.gnu.org/licenses/>.
  =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treelibc.h"

char *pppKeysValues[][9] = { /* test data */
	{ "Scalia", "Kennedy", "Thomas", "Ginsburg", "Breyer", "Roberts", "Alito, Jr", "Sotomayor", "Kagan" },
	{ "Reagan", "Reagan", "Bush", "Clinton", "Clinton", "W. Bush", "W. Bush", "Obama", "Obama" },
};

static int compareStr(const void* pKeyOld, const void* pKeyNew) { /* string compare function */
return(strcmp(pKeyOld, pKeyNew));
}

static int compareDescendUL(const void* pKeyOld, const void* pKeyNew) { /* number compare function */
	const unsigned long *ulOld = pKeyOld, *ulNew = pKeyNew;
	if((*ulOld) < (*ulNew)) /* flip relational for ascend */
		return(1);
	if((*ulOld) > (*ulNew)) /* flip relational for ascend */
		return(-1);
return(0);
}

static void printData(Tree *pTree, int iType) { /* shared general purpose function */
	void **pp;
	unsigned long ul, ulLen = treeLength(pTree);

	printf("Length: %lu\n", ulLen); /* get number of keys */

	puts("--- order ---");
	pp = treeArray(pTree); /* get array of keys */
	for(ul = 0; ul < ulLen; ul++) {
		if(iType)
			printf("%lu - %s\n", *((unsigned long*)pp[ul]), (char*)treeValue(pTree, pp[ul]));
		else
			printf("%s - %s\n", (char*)pp[ul], (char*)treeValue(pTree, pp[ul]));
	}

	puts("--- sorted ---");
	pp = treeArraySorted(pTree); /* get array of sorted keys */
	for(ul = 0; ul < ulLen; ul++) {
		if(iType)
			printf("%lu - %s\n", *((unsigned long*)pp[ul]), (char*)treeValue(pTree, pp[ul]));
		else
			printf("%s - %s\n", (char*)pp[ul], (char*)treeValue(pTree, (char*)pp[ul]));
	}
}

int main(void) {
	Tree tree, t2;
	unsigned long ul, ulLen = sizeof(*pppKeysValues) / sizeof(char*);

	/* ----- TEST CASE 1 INSERT ONLY ADDRESSES OF KEYS AND VALUES  ---- */
	if(treeInit(&tree, compareStr) == NULL) {
		fprintf(stderr, "ERROR: initialization failed!");
		return EXIT_FAILURE;
	}
	puts("--- insert ----------------------------");
	for(ul = 0; ul < ulLen; ul++) /* insert key copy and value address */
		treeInsert(&tree, pppKeysValues[0][ul], 0, pppKeysValues[1][ul], 0);
	printData(&tree, 0);

	/* --------- TEST CASE 2 INSERT KEY COPIES AND VALUE COPIES ---------- */
	treeFree(&tree); /* free all memory used by tree */
	treeInit(&tree, compareStr);
	puts("--- insert ----------------------------");
	for(ul = 0; ul < ulLen; ul++) /* insert copy of both key and value */
		treeInsert(
			&tree, pppKeysValues[0][ul], strlen(pppKeysValues[0][ul]),
			pppKeysValues[1][ul], strlen(pppKeysValues[1][ul])
		);
	printData(&tree, 0);

	/*- ---------------------- TEST CASE 3 UPDATE ------------------------ */
	puts("--- update ----------------------------");
	printf("Update: %s - %s\n", "Thomas", "H. W. Bush"); /* update value */
	treeUpdate(&tree, "Thomas", "H. W. Bush", strlen("H. W. Bush"));
	printData(&tree, 0);

	/* ---------------------- TEST CASE 4 DELETE ---------------------- */
	puts("--- delete ----------------------------");
	printf("Delete: %s\n", "Roberts"); /* delete the Chief justice */
	treeDelete(&tree, "Roberts");
	printData(&tree, 0);

	/* ---------------------- TEST CASE 5 RE-INSERT ---------------------- */
	puts("--- re-insert ---------------------------"); /* re-insert the chief justice */
	printf("Insert: %s - %s\n", "Roberts", "W. Bush"); /* re-insert the chief justice */
	treeInsert(&tree, "Roberts", strlen("Roberts"), "W. Bush", strlen("W. Bush"));
	printData(&tree, 0);

	/* - TEST CASE 6 ADD SECOND TREE, USE VALUES AS KEYS AND PASS NULL AS VALUES - */
	treeInit(&t2, compareStr);
	puts("--- insert ----------------------------");
	for(ul = 0; ul < ulLen; ul++)
		treeInsert(&t2, pppKeysValues[1][ul], 0, NULL, 0); /* inserts only unique keys */
	printData(&t2, 0);

	/* - TEST CASE 7 FREE SECOND TREE AND RE-INITIALIZE WITH NUMBERS WITHOUT VALUE COPY - */
	treeFree(&t2); /* free all memory used by tree */
	treeInit(&t2, compareDescendUL); /* comparison implementation creates descending order */
	puts("--- insert ----------------------------");
	for(ul = 1; ul <= ulLen; ul++) /* local variables must use copy feature by passing size */
		treeInsert(&t2, &ul, sizeof(unsigned long), pppKeysValues[0][ul - 1], 0);
	printData(&t2, 1);

	treeFree(&tree);
	treeFree(&t2);

	puts("FINISHED!");

return EXIT_SUCCESS;
}
