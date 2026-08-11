#pragma once
//skbf_user.h - find solution of a puzzle and UAs in specific contitions

/*
Based on code posted to <http://forum.enjoysudoku.com/3-77us-solver-2-8g-cpu-testcase-17sodoku-t30470.html>
by user zhouyundong_2012.
The copyright is not specified.

This DLL first and main task is to  find the unique solution of a valid puzzle.

The DLL is also used, when the solution grid is known, to find a significant set of unavoidable sets in specific conditions

The DLL does not decide what set has to be produced, but offers services for an easy harvest of relevant uas.

The basic process to produce unavoidable sets with a known solution grid is quite simple:
 the start is a grid with multiple solutions,
 for each solution not equal to the knwon solution grid, all cells differing from the solution form an unavoidable set.
This process performs well if the given have a limited number of solution, although this can be thousands.
And the process can be filtered against redundant UAs if the list of already known UAs is given to the process.

The most common use is to assign all digits but 2 or 3, but if needed, the number of unassigned digits can be pushed to 4/5
Another use can be to search part of the grid. I often consider to assign all the grid but 4 boxes in square.


As in other DLLs, an exchange area is there to send back results to the user.

struct ZHE {// exchange area for the caller
	BF128 tpat[100],wpat;
	int* grid0;// user grid
	int npat;
	int gsol[82];
	}
}

gsol is the solution grid for a sudoku (one solution for the puzzle)
tpat /npat delivers the list of UAs in a 128 bits fields (4 x 32 bits)
where the three bands (3 x 27 bits) are located in the 3 first 32 bits integers.
This is the way puzzles are processed in this process.
Note: tpat is open to 100 UAs, but the limit in the code is currently 50


The DLL has the following entries  entries 
=======================
ZHE* SkbfGetZhePointer(); returning the pointer to the exchange area
======================= the classical brute force check
int SkbfCheckValidityQuick(char* puzzle);
  return 	0 if the puzzle is not valid, 
		1 if the solution is unique
		2 i the puzzle has mre than one solution
======================= help to get back doors 

int SkbfIsSolvedInOne(char* puzzle, char* solved);
  return 1 if puzzle is solved without guess 
		 0 in other cases
 char * solved if a 81 bit field with unsoved cells set to 1
 This field is loaded if the puzzle is valid and not solved without guess
 
 ======================= help to solve quick in 2 steps (tridagon finder)
 

int SkbfSkbfDoNoGuess(char* puzzle, BF128** unsr);
  return 1 if puzzle is solved without guess 
		 0 in other cases
 BF128** unsr if the 81 bit field unsolved cells at the end
 This field is loaded if the  not solved without guess
 
 int SkbfEndNoGuess(int* sol);
 finish the brute force as in {int SkbfCheckOkForSol(int* sol, GINT16* t, int n, int withknown);}
 where with known=0
 

===================   now entries leading to an  unavoidable sets  list 
=========================== known list of UAs
void SkbfKnown(BF128 * t, int n);
This call, (if possible) send to the brute force the list of known UAs relevant in the search 
to filter redundant UAs or UAs having a subset
It is the caller responsability to send the right list, but a "outfield" list will have no effect.

=============================== search assigning all digits but..
int SkbfGetFloorsSols(int* sol ,int floors, int withknown);
floors is a 9 bits field telling which digits are unknown (2 3 digits, maybe more )
 eg .1..1...  digits 2 and 5  
	integer value 	2^1+2^4 or 
			1<<0+1<<4 or
    			18
int * sol is  int [81] set of values 0-8 for the solution grid

At the end, tpat contains the list of unavoidable sets ordered from the smallest (in size) to the biggest, with no subset/superset.
if "withknown is set to 1, each solution is checked agains the list of UAs previously given in SkbfKnown...
extern "C" __declspec(dllexport) ZHE * SkbfGetZhePointer();
============================== search in any given status 
int SkbfCheckOkForSol(int* sol, GINT16* t, int n, int withknown);

Although this is not the target, this process can be called with a valid grid; then, the response is "no unavoidalbe set"

It is a process to call with a puzzle very close to a valid one.
I made the test with 17 clues valid puzzles reduced to 16, this can have a very big number of solutions. 
int* sol is the known solution as above.

GINT16* t, int n  is the list of clues given in the internal format of the process (after analysis of char * puzzle)
GINT16 is a 16 bits field having here 8 low bits for the cell and 8 high bits for the digit
	eg for a cell c (0-80) and adigit d (0-8) the clue is (uint16_t)(c | (d << 8));

the return value is the number of UAs
and the UAs are as above, in the limit of the maximum (50) number of UAs
Again, a filter on known UAs is available. A call with a grid assumed valid will have no "known UA" available.

*/
#include "bf128.h"
#ifdef TESTNEWBF
ZHE * pzhe;
#else
struct ZHE {// exchange area for the caller
	BF128 tpat[100],wpat;
	int* grid0;// user grid
	int npat;
	int gsol[82];
	
}*pzhe;
#endif
/*
// if not yet there
typedef union GINT16_t {
	uint16_t   u16;
	uint8_t    u8[2];
} GINT16
*/
extern "C" __declspec(dllimport) int SkbfDoNoGuess(char* puzzle, BF128** unsr);
extern "C" __declspec(dllimport) int SkbfEndNoGuess(int* sol);
extern "C" __declspec(dllimport) int SkbfCheckValidityQuick(char* puzzle);
extern "C" __declspec(dllimport) int SkbfIsSolvedInOne(char* puzzle, char* solved);
extern "C" __declspec(dllimport) int SkbfGetFloorsSols(int* puzzle, int floors, int withknown);
//extern "C" __declspec(dllimport) int SkbfCheckOkForSol(int* sol, GINT16 * t, int n, int withknown);
// unexplained problem here temporary discarded
extern "C" __declspec(dllimport) ZHE * SkbfGetZhePointer();
extern "C" __declspec(dllimport) void SkbfKnown(BF128 * t, int n);

