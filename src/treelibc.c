/*
 =============================================================================
 Name        : treelibc.c
 Author      : David T. Silvers Sr.
 Contact     : davidtsilvers@aol.com
 Created     : 2014-02-03
 Updated     : 2014-02-27
 Version     : 1.101
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treelibc.h"

typedef struct node {
	size_t sizeTkey;
	void *pKey;
	size_t sizeTvalue;
	void *pValue;
	enum { NODE_RED, NODE_BLACK } color;
	struct node *pPrev, *pNext, *pParent, *pRight, *pLeft;
} Node;

static Node* initNode(Tree *, Node *, void *, size_t, void *, size_t); /* Allocates memory for each node */
static void copyKeyValue(Tree *, Node *, void *, size_t, void *, size_t); /* General purpose copy key/value */
static void copyValue(Tree *, Node *pNode, void*, size_t); /* General purpose copy value */
static void balanceTree(Tree *, Node *); /* Entry point to add Red-Black Tree to Binary Tree */
static Node* resolveRB(Tree *, Node *, Node *); /* Consolidates shared left/right and Red-Black logic */
static void rotateRightRB(Tree *, Node *); /* Rotate Red-Black Tree to right when tree needs re-balancing */
static void rotateLeftRB(Tree *, Node *); /* Rotate Red-Black Tree to left when tree needs re-balancing */
static Node* getNodeByKey(Tree *, const void *); /* General purpose called by various functions */
static void removeNode(Node *); /* General purpose to remove node from tree */
static void spliceLeft(Node *); /* General purpose to assign node's parent to node's left child */
static void spliceRight(Node *);  /* General purpose to assign node's parent to node's right child */
static void resetList(Tree *, Node *); /* General purpose to remove node from list */

unsigned long treeLength(Tree *pTree) { return(pTree->ulTreeLen); }

Tree* treeInit(Tree *pTree, PFCMP pfCmp) {
	if((pTree == NULL) || (pfCmp == NULL))
		return(NULL);
	pTree->pfCmp = pfCmp;
	pTree->ulTreeLen = 0;
	pTree->pr = pTree->ph = pTree->pt = pTree->ppArray = pTree->ppArraySorted = NULL;
return(pTree);
}

void *treeValue(Tree *pTree, const void *pKey) {
	Node *pNode;
	if((pTree != NULL) && (pKey != NULL) && ((pNode = getNodeByKey(pTree, pKey)) != NULL))
		return(pNode->pValue);
return(NULL);
}

void** const treeArray(Tree *pTree) {
	Node *pNode;
	unsigned long l, lIndex = 0;
	if((pTree == NULL) || (pTree->ulTreeLen <= 0))
		return NULL;
	if(pTree->ppArray != NULL)
		free(pTree->ppArray);
	pTree->ppArray = malloc(pTree->ulTreeLen * sizeof(void*));
	for(l = 0; l < pTree->ulTreeLen; l++)
		pTree->ppArray[l] = NULL;
	for(pNode = pTree->ph; pNode != NULL; pNode = pNode->pNext)
		pTree->ppArray[lIndex++] = pNode->pKey;
return(pTree->ppArray);
}

void** const treeArraySorted(Tree *pTree) {
	unsigned long l, lIndex = 0;
	Node *p, *pN, *pNode = pTree->pr;
	if((pTree == NULL) || (pTree->ulTreeLen <= 0))
		return NULL;
	if(pTree->ppArraySorted != NULL)
		free(pTree->ppArraySorted);
	pTree->ppArraySorted = malloc(pTree->ulTreeLen * sizeof(void*));
	for(l = 0; l < pTree->ulTreeLen; l++)
		pTree->ppArraySorted[l] = NULL;
	while(pNode != NULL) {
		for(p = pNode; p != NULL; p = p->pLeft)
			pNode = p;
		pTree->ppArraySorted[lIndex++] = pNode->pKey;
		pN = pNode;
		pNode = pNode->pRight;
		if(pNode == NULL) {
			for(pNode = pN->pParent; pNode != NULL; pNode = pNode->pParent) {
				if(pNode->pRight != pN) {
					if(pNode->pLeft != NULL)
						pTree->ppArraySorted[lIndex++] = pNode->pKey;
					if(pNode->pRight != NULL) {
						pNode = pNode->pRight;
						break;
					}
				}
				pN = pNode;
			}
		}
	}
return(pTree->ppArraySorted);
}

int treeUpdate(Tree *pTree, const void *pKey, void *pValue, size_t sizeTvalue) {
	Node *pNode;
	if((pTree != NULL) && (pKey != NULL) && ((pNode = getNodeByKey(pTree, pKey)) != NULL)) {
		copyValue(pTree, pNode, pValue, sizeTvalue);
		return(1);
	}
return(0);
}

int treeDelete(Tree *pTree, const void *pKey) {
	int iRet = 0, iListSettled = 0;
	Node *pNode;
	if((pTree == NULL) || (pKey == NULL))
		return 0;
	if((pNode = getNodeByKey(pTree, pKey)) != NULL) {
		if((pNode->pLeft == NULL) && (pNode->pRight == NULL)) {
			if(pNode == pTree->pr) {
				treeFree(pTree);
				return(1);
			}
			removeNode(pNode);
		} else if((pNode->pLeft != NULL) && (pNode->pRight != NULL)) {
			Node *p, *pN;
			for(p = pNode->pLeft; p != NULL; p = p->pRight)
				pN = p;
			copyKeyValue(pTree, pNode, pN->pKey, pN->sizeTkey, pN->pValue, pN->sizeTvalue);
			resetList(pTree, pNode);
			pNode->pPrev = pN->pPrev;
			pNode->pNext = pN->pNext;
			if(pNode->pPrev != NULL)
				pNode->pPrev->pNext = pNode;
			if(pNode->pNext != NULL)
				pNode->pNext->pPrev = pNode;
			iListSettled = 1;
			if(pN->pLeft != NULL) {
				spliceLeft(pN);
				balanceTree(pTree, pN->pLeft);
			} else
				removeNode(pN);
			pNode = pN;
		} else if(pNode->pLeft != NULL) {
			if(pNode == pTree->pr) {
				pTree->pr = pNode->pLeft;
				((Node*)pTree->pr)->color = NODE_BLACK;
			} else {
				spliceLeft(pNode);
				balanceTree(pTree, pNode->pLeft);
			}
		} else {
			if(pNode == pTree->pr) {
				pTree->pr = pNode->pRight;
				((Node*)pTree->pr)->color = NODE_BLACK;
			} else {
				spliceRight(pNode);
				balanceTree(pTree, pNode->pRight);
			}
		}
		if(!iListSettled)
			resetList(pTree, pNode);
		if((pNode->pValue != NULL) && (pNode->sizeTvalue > 0))
			free(pNode->pValue);
		if(pNode->sizeTkey > 0)
			free(pNode->pKey);
		free(pNode);
		pTree->ulTreeLen--;
		iRet = 1;
	}
return(iRet);
}

void treeFree(Tree *pTree) {
	Node *pN, *pNode = pTree->ph;
	while(pNode != NULL) {
		pN = pNode;
		pNode = pNode->pNext;
		if((pN->pValue != NULL) && (pN->sizeTvalue > 0))
			free(pN->pValue);
		if(pN->sizeTkey > 0)
			free(pN->pKey);
		free(pN);
	}
	if(pTree->ppArray != NULL)
		free(pTree->ppArray);
	if(pTree->ppArraySorted != NULL)
		free(pTree->ppArraySorted);
	treeInit(pTree, pTree->pfCmp);
return;
}

int treeInsert(Tree *pTree, void *pKey, size_t sizeTkey, void *pValue, size_t sizeTvalue) {
	int iCmp;
	Node *pN, *pNode;
	if((pTree == NULL) || (pKey == NULL))
		return(0);
	if((pNode = initNode(pTree, pTree->pr, pKey, sizeTkey, pValue, sizeTvalue)) != NULL) {
		pNode->color = NODE_BLACK;
		pTree->pr = pNode;
		return(1);
	}
	pNode = pTree->pr;
	while(pNode != NULL) {
		if((iCmp = pTree->pfCmp(pNode->pKey, pKey)) < 0) {
			if((pN = initNode(pTree, pNode->pRight, pKey, sizeTkey, pValue, sizeTvalue)) != NULL) {
				pNode->pRight = pN;
				pN->pParent = pNode;
				balanceTree(pTree, pN);
				break;
			}
			pNode = pNode->pRight;
		} else if(iCmp > 0) {
			if((pN = initNode(pTree, pNode->pLeft, pKey, sizeTkey, pValue, sizeTvalue)) != NULL) {
				pNode->pLeft = pN;
				pN->pParent = pNode;
				balanceTree(pTree, pN);
				break;
			}
			pNode = pNode->pLeft;
		} else
			return(0);
	}
return(1);
}

static Node* initNode(
	Tree *pTree, Node *pNode, void *pKey, size_t sizeTkey, void *pValue, size_t sizeTvalue
) {
	if(pNode != NULL)
		return(NULL);
	pTree->ulTreeLen++;
	pNode = malloc(sizeof(Node));
	pNode->color = NODE_RED;
	pNode->sizeTkey = sizeTkey;
	pNode->sizeTvalue = sizeTvalue;
	pNode->pKey = pNode->pValue = NULL;
	pNode->pParent = pNode->pRight = pNode->pLeft = pNode->pNext = NULL;
	pNode->pPrev = pTree->pt;
	if(pTree->ph == NULL)
		pTree->ph = pTree->pt = pNode;
	else
		((Node*)pTree->pt)->pNext = pNode;
	pTree->pt = pNode;
	copyKeyValue(pTree, pNode, pKey, sizeTkey, pValue, sizeTvalue);
return(pNode);
}

static void copyKeyValue(
	Tree *pTree, Node *pNode, void *pKey, size_t sizeTkey, void *pValue, size_t sizeTvalue
) {
	if((pNode->pKey != NULL) && (pNode->sizeTkey > 0))
		free(pNode->pKey);
	if(sizeTkey > 0) {
		pNode->pKey = malloc(sizeTkey + 1);
		memcpy(pNode->pKey, pKey, sizeTkey);
		((char*)pNode->pKey)[sizeTkey] = '\0';
	} else
		pNode->pKey = pKey;
 	copyValue(pTree, pNode, pValue, sizeTvalue);
return;
}

static void copyValue(Tree *pTree, Node *pNode, void *pValue, size_t sizeTvalue) {
	if((pNode->pValue != NULL) && (pNode->sizeTvalue > 0))
		free(pNode->pValue);
	if(sizeTvalue > 0) {
		pNode->pValue = malloc(sizeTvalue + 1);
		memcpy(pNode->pValue, pValue, sizeTvalue);
		((char*)pNode->pValue)[sizeTvalue] = '\0';
	} else
		pNode->pValue = pValue;
return;
}

static void balanceTree(Tree *pTree, Node *pNode) {
	if((pNode != NULL ) && (pNode->color == NODE_RED)) {
		while((pNode != pTree->pr)
		&& (pNode->pParent->color == NODE_RED)
		&& (pNode->pParent->pParent != NULL)) {
			if(pNode->pParent == pNode->pParent->pParent->pLeft)
				pNode = resolveRB(pTree, pNode, pNode->pParent->pParent->pRight);
			else
				pNode = resolveRB(pTree, pNode, pNode->pParent->pParent->pLeft);
		}
		pNode = pTree->pr;
		if((pNode->color == NODE_RED)
		&& (((pNode->pLeft != NULL) && (pNode->pLeft->color == NODE_RED))
		|| ((pNode->pRight != NULL) && (pNode->pRight->color == NODE_RED))))
			pNode->color = NODE_BLACK;
	}
return;
}

static Node* resolveRB(Tree *pTree, Node *pNode, Node *pUncle) {
	if((pUncle != NULL) && (pUncle->color == NODE_RED)) {
		pNode->pParent->pParent->color = NODE_RED;
		pNode->pParent->color = NODE_BLACK;
		pUncle->color = NODE_BLACK;
		pNode = pNode->pParent->pParent;
	} else {
		char parentOnLeft = (pNode->pParent == pNode->pParent->pParent->pLeft) ? 1 : 0;
		if(parentOnLeft) {
			if(pNode == pNode->pParent->pRight) {
			   pNode = pNode->pParent;
			   rotateLeftRB(pTree, pNode);
			}
		} else {
			if(pNode == pNode->pParent->pLeft) {
			   pNode = pNode->pParent;
			   rotateRightRB(pTree, pNode);
			}
		}
		if(pNode->pParent != NULL) {
			pNode->pParent->color = NODE_BLACK;
			if(pNode->pParent->pParent != NULL) {
				pNode->pParent->pParent->color = NODE_RED;
				if(parentOnLeft)
					rotateRightRB(pTree, pNode->pParent->pParent);
				else
					rotateLeftRB(pTree, pNode->pParent->pParent);
			}
		}
	}
return(pNode);
}

static void rotateLeftRB(Tree *pTree, Node *pNode) {
    Node *y, *x = pNode;
    y = x->pRight;
    x->pRight = y->pLeft;
    if ( y->pLeft != NULL )
        y->pLeft->pParent = x;
    y->pParent = x->pParent;
    if(x->pParent != NULL) {
        if(x == x->pParent->pLeft )
            x->pParent->pLeft = y;
        else
            x->pParent->pRight = y;
    }
    else
    	pTree->pr = y;
    y->pLeft = x;
    x->pParent = y;
return;
}

static void rotateRightRB(Tree *pTree, Node *pNode) {
    Node *y, *x = pNode;
    y = x->pLeft;
    x->pLeft = y->pRight;
    if ( y->pRight != NULL )
        y->pRight->pParent = x;
    y->pParent = x->pParent;
    if(x->pParent != NULL) {
    	if(x == x->pParent->pRight )
            x->pParent->pRight = y;
        else
            x->pParent->pLeft = y;
    }
    else
    	pTree->pr = y;
    y->pRight = x;
    x->pParent = y;
return;
}

static void spliceLeft(Node *pNode) {
	if(pNode->pParent != NULL) {
		pNode->pLeft->pParent = pNode->pParent;
		if(pNode->pParent->pLeft == pNode)
			pNode->pParent->pLeft = pNode->pLeft;
		else
			pNode->pParent->pRight = pNode->pLeft;
	}
return;
}

static void spliceRight(Node *pNode) {
	if(pNode->pParent != NULL) {
		pNode->pRight->pParent = pNode->pParent;
		if(pNode->pParent->pRight == pNode)
			pNode->pParent->pRight = pNode->pRight;
		else
			pNode->pParent->pLeft = pNode->pRight;
	}
}

static void removeNode(Node *pNode) {
	if(pNode->pParent != NULL) {
		if(pNode->pParent->pLeft == pNode)
			pNode->pParent->pLeft = NULL;
		else
			pNode->pParent->pRight = NULL;
	}
return;
}

static Node* getNodeByKey(Tree *pTree, const void *pKey) {
	if(pKey != NULL) {
		int iCmp;
		Node *pNode = pTree->pr;
		while(pNode != NULL) {
			if((iCmp = pTree->pfCmp(pNode->pKey, pKey)) == 0)
				return(pNode);
			pNode = (iCmp > 0) ? pNode->pLeft : pNode->pRight;
		}
	}
return(NULL);
}

static void resetList(Tree *pTree, Node *pNode) {
	if(pNode->pPrev != NULL) {
		if(pNode->pNext != NULL) {
			pNode->pPrev->pNext = pNode->pNext;
			pNode->pNext->pPrev = pNode->pPrev;
		} else {
			pNode->pPrev->pNext = NULL;
			pTree->pt = pNode->pPrev;
		}
	} else {
		pNode->pNext->pPrev = NULL;
		pTree->ph = pNode->pNext;
	}
}
