/*
 =============================================================================
 Name        : treelibc.h
 Author      : David T. Silvers Sr.
 Contact     : davidtsilvers@aol.com
 Created     : 2014-02-03
 Updated     : 2014-02-14
 Version     : 1.10
 License     : GNU LGPL
 Description : Associative Balanced Tree Container with no recursive limits.
               Generic implementation requires user supplied compare function.
               Key size of 0 are address assignments to user allocated memory.
               Key sizes greater than 0 have their keys copied into tree.
               Value size of 0 to treeInsert() and treeUpdate() are address
               assignments to user allocated memory or user supplied NULL.
               Value sizes greater than 0 have their values copied into tree.

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
#ifndef _TREELIBC_H_
#define	_TREELIBC_H_
#ifdef __cplusplus
extern "C" {
#endif

/* user supplied comparison function for keys */
typedef int (*PFCMP)(const void *, const void *); /* Returns: -1 = LT; 0 = EQ; 1 = GT */

typedef struct tree { /* convenience structure to allow for multiple trees in process */
	PFCMP pfCmp; /* points to user supplied comparison function. */
	unsigned long ulTreeLen; /* returned by treeLength() */
	void **ppArray; /* returned by treeArray() */
	void **ppArraySorted; /* returned by treeArraySorted() */
	void *pr; /* internal use only */
	void *ph; /* internal use only */
	void *pt; /* internal use only */
} Tree;

/* Global function declarations for external usage. CRUD: Create, Read, Update and Delete */
extern Tree* treeInit(Tree *pTree, PFCMP pfCmp); /* Return: NULL = fail */
extern int treeInsert(Tree *pTree, void *pKey, size_t sizeTkey, void *pValue, size_t sizeTvalue); /* Return: 0 = fail; 1 = insert */
extern unsigned long treeLength(Tree *pTree); /* Return number of unique inserted keys for length of arrays */
extern void** const treeArray(Tree *pTree); /* get array of keys in order of insertion. Return: NULL = fail */
extern void** const treeArraySorted(Tree *pTree); /* get array of sorted keys. Return: NULL = fail */
extern void* treeValue(Tree *pTree, const void *pKey); /* get value from given key. Return: NULL = fail */
extern int treeUpdate(Tree *pTree, const void *pKey, void *pValue, size_t sizeTvalue); /* Return: 0 = fail; 1 = update */
extern int treeDelete(Tree *pTree, const void *pKey); /* delete key. Return: 0 = fail; 1 = deleted */
extern void treeFree(Tree *pTree); /* free internally allocated memory for given tree */

#ifdef __cplusplus
}
#endif
#endif
