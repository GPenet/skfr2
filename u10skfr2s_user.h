#pragma once
/*
u10skfr2_user.h - sudoku fast rating second version

This DLL, for a given valid sudoku,  finds the ER EP ED ratings similar to Sudoku Explainer ratings.
This DLL also accepts a sukaku start PM 

The code uses the brute force DLL to see if the proposed entry is a valid sudoku/sukaku with a unique solution.
Note: at the beginning, this check is supposed to be done by the caller for a sukaku entry.

 ==================================== exchange area
 
 the DLL works using an internal exchange area 
 struct SKFR2II{
	char mode_sudoku_sukaku;// 0 sudoku, 1 sukaku
	char puz[82];		// given puz string mode (dot for a non given)
	char sol[82];		// sol string given for a sukaku
	int sukaku_pm[9][3];// in sukaku mode, the 9 digits 81 bits fields in 3x27 bits mode
	// rating is given in integer form 10 to 120 for SE 1.0 to 12.0
	// other values of ER are used for "false returns
	// 0 not valid 1 multiple solutions 2 below min ER filter 
	// 1xxx not solved, xxx last ER seen
	int Er,EP,ED;		// ratings as of Sudoku Explainer
	// filters for the process
	int min_er, // all puzzles  rating below receive Er=2.
	only_er;	//  If only_er process uses the quickest path below er at start
	int min_ed; // this is to look for diamonds or quasi diamonds 
	int max_er; // all unsolved at this level come back with rating 1xxx
 };
 
 The first call must be to get the pointer to this area. In this call, the DLL get the pointers to other DLLs.
 
 
 ========================== entries

SKFR2II* Skfr2GetP();
 first and compulsory call to get the exchange pointer.
 Null pointer is error
 
int Skfr2Puz(char* p);
  usual call to get the ratyings.
  return 1 if error somewhere

int skfr2kaku(int opt);// same for a sukaku described in the exchange area
//if opt 1 (not yet available, the check for a unique solution is done. 
At the start, this is done by the caller, the DLL makes only structural controls
 
*/
 struct SKFR2II{
	char mode_sudoku_sukaku;	// 0 for sudoku, 1 for sukaku
	char puz[82];				// given puz string mode (dot for a non given)
	char sol[82];				// sol string given for a sukaku
	int sukaku_pm[9][3];		// in sukaku mode, the 9 digits 81 bits fields in 3x27 bits mode
	int Er,EP,ED;				// rating as of Sudoku Explainer
	// filters for the process
	int min_er,only_er,min_ed,max_er;  
 };
