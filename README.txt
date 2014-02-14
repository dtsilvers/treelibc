=============================================================================
 Name        : treelibc SEE: src/treelibc.c AND src/treelibc.h 
               SEE ALSO: dist/treelibc_test.c AND dist/treelibc.h
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
  Rev 1.10 
     
     This is not a bug fix as much as it is a feature enhancement to make 
     the functionality more of what one may have seen in other languages. 
     
  Summary: 
     
     Deletions consistently shift any subsequent array elements to the left. 

  Details:
           
     Previously, when a key was deleted, the functionality took the fastest
     route of removing the key regardless of the original insertion order. 
     Since in all cases treeArray(..) should be called after every deletion, 
     the deletion ignored the original insertion order, because regardless
     of how a key is removed, there are less elements so it's not original. 
     A version 1.00 deletion would at times maintain a similar insertion  
     order to the original insertion, then other times it was faster to just
     copy key/value data from another element without resetting all pointers,
     although, since performance tradeoff is negligible it was worth revision.
  
  Code changes: treelibc.c, treelibc_test.c
  
    treelibc.c: SEE ABOVE
  
      EDIT: int treeDelete(Tree *pTree, const void *pKey); 
  
  	  ADD: static void resetList(Tree *, Node *); /* general purpose function */
  	
    treelibc_test.c: print more detail
  
      EDIT: int main(void)  
     
  -----------------------------------------------------------------------------
  Project developed in Eclipse-Kepler-CPP with MinGW32 on Windows 8.
  -----------------------------------------------------------------------------

  libtreelibc.a is built using treelibc.c and treelibc.h 

  Example: 
  
    gcc -O3 -Wall -c -fmessage-length=0 -o "src\\treelibc.o" "..\\src\\treelibc.c" 

    ar -r libtreelibc.a "src\\treelibc.o" 
  
  -----------------------------------------------------------------------------

  To link with your code, include treelibc.h and link in libtreelibc.a  

  Example:

    gcc -O3 -Wall -c -fmessage-length=0 -o "src\\treelibc_test.o" "..\\src\\treelibc_test.c" 
    
    gcc -L../lib -o treelibc_test.exe "src\\treelibc_test.o" -ltreelibc  

  -----------------------------------------------------------------------------

  If using Unix or Linux, follow a similar procedure outlined in the above steps.
