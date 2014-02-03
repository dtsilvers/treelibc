=============================================================================
 Name        : treelibc SEE: src/treelibc.c AND src/treelibc.h 
               SEE ALSO: dist/treelibc_test.c AND dist/treelibc.h
 Author      : David T. Silvers Sr.
 Contact     : davidtsilvers@aol.com
 Date        : 2014-02-03
 Version     : 1.00
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
