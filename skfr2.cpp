
/*
My standard start using maingp as command line analyser
contains the entry G0() called by main
*/
//#define SEROUT 40
#define SEROUTEND 120
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define INCLUDE_DEBUGGING_CODE
#define SK0FDEBUGGING
#include <stdlib.h>
#include <sys/timeb.h>
using namespace std;
// nothing to do here
#include "maingp_cpp.h"
#include "sk0\sk0_cpp_lookup.h" 
#include "sk0\sk0_cpp_tpermbase.h" 
#include "sk0\sk0_cpp_debugging.h"
#include "sk0\sk0_cpp_floors.h"
#include "sk0\sk0_cpp_misc_lookup.h"
#include "bf128.h"
#include "bf128_cpp.h"
#include "sk0\sk0_cpp_t128.h"
#include "sk0\sk0_cellname.h"
#include "dllusers\u06skbf_user.h"
/*

		// relative cells in unit per digit
	BF128 	digits_band[9],
		digits_stacks[9],
		digits_band_box[9];
		// cells unsolved  per unit (band is unsolved cells)
	BF128  free_stacks, free_band_box; // 81 cells in each case
*/
T128 bf128_3x = { BIT_SET_2X,BIT_SET_27 };

#include "UDL\U02\sku02_structs.h"
#include "UDL\U02\sku02_solve.h"
#include "UDL\U02\sku02_minis.h"
#include "UDL\U02\sku02_sets.h"

struct COUNTSTATUS {// has "n" cells or digits active
	BF128 ccells; // 3x27 
	uint32_t cunit, // count cells per unit
		cdigits_units[9];// digit/count cells per unit
};



void C0() { //  create t512v and seen cells 
	for (int i = 0; i < 81; i++) {
		BF128 w(cell_z3x[i]); // cells seen by the target
		int tc[20], nt = 0;
		w.Table3X27(tc);
		cout << '{';
		for (int j = 0; j < 20; j++) cout << tc[j] + 1 << ',';
		cout << '},' << endl;;

	}
	return;
	for (int i = 0; i < 512; i++) {
		int n = 0;
		cout << '"';
		for (int j = 0, bit = 1; j < 9; j++, bit <<= 1)
			if (bit & i)cout << j + 1; else n++;
		for (int j = 0; j <n; j++)cout << " ";
		cout << '"'<<',';
		if (!(i+1 & 7)) cout << endl;
	}

}

void C1() {
}

// c2 work on a file of puzzles  if( v2 1 image at strt
// sgo.vx[4] 
//  0 image atfer singles
//  1 backdoor 1
//  2 backdoor 2

void C2() {
	solve.opp = sgo.bfx[8];
	cout << " C2_ file of puzzles  " << sgo.vx[4] << endl;
	pzhe = SkbfGetZhePointer();
	char* ze = finput.ze;
	uint32_t npuz = 0;
	while (finput.GetLigne()) {
		if (strlen(ze) < 81)continue;// no empty line
		if ((!sgo.vx[5]) && npuz++ < sgo.vx[0])continue;

		if (solve.opp&1)	cout << ze << " npuz=" << npuz << endl;
		if(solve.Init(ze))goto  end;
		if (sgo.vx[4])solve.SinglesAtStart();

		switch (sgo.vx[4]) {
			//____________________ pm at start
		case 0: {cout << ze << " npuz=" << npuz << endl;
				solve.sv81w.ImageCandidats(1); break;
			}
			//______________________ backdoor1
		case 1: {// backdoor 1
				int ir = solve.Backdoor1();
				if (ir == 82) break; // stte at start
				if (sgo.vx[5]) {// fout backdoor1
					if (ir >= 0)
						fout1 << ze << ";" << ir + 1 << ";" << solve.g0[ir] + 1 << endl;
					else cout << ze << ";more" << endl;
				}
				else {// fout more than backdoor1
					if (ir < 0)fout1 << ze << endl;
				}
				break;
			}
			  //_________________ backdoor 2
		case 2: {// backdoor 2
				cout << ze << " npuz=" << npuz << "entry back2" << endl;
				uint32_t ir = solve.Backdoor2();
				cout << " bakk ir=" << ir << endl;
				if (ir == 0xfe) break; // stte at start or backdoor 1
				if (sgo.vx[5]) {// fout backdoor 2
					if (ir !=0xff) {
						int ir1 = ir & 0xff, ir2 = ir >> 8;
						fout1 << ze
							<< ";" << ir1 + 1 << ";" << solve.g0[ir1] + 1
							<< ";" << ir2 + 1 << ";" << solve.g0[ir2] + 1
							<< endl;
					}
					else cout << ze << ";more" << endl;
				}
				else {// fout more than backdoor 2
					if (ir < 0)fout1 << ze << endl;
				}
				break;
			}
			  //______________________ TE1
		case 3: {// T&E 1 filter
			if (sgo.vx[2] > 1)
				cout << ze << " npuz=" << npuz << endl;
			if (sgo.vx[2] > 1)solve.sv81w.ImageCandidats(1); //break;
			if (solve.sv81w.unsolved_cells.isEmpty()) break;
			int ir = solve.IsTE1();
			if (ir < 0) break; // stte at start
			if (!ir) break; // t&e1
			fout1 << ze << endl;
			break;
		}

		case 10: // solve more 
			//solve.SolveQuick(ze);
			break;
		}// end switch

	end:

		if ((!sgo.vx[5]) && npuz >= sgo.vx[1])break;
	}
	cout << "end of file   " << endl;


}

// c3 work on a file of ranks
void C3() {
	cout << " C3_  file of ranks case  "<< sgo.vx[4] 
		<< " npuz " << sgo.vx[0]<<" " << sgo.vx[1] << endl;
	char* ze = finput.ze;
	//int g[81];
	uint32_t npuz = 0;;

	while (finput.GetLigne()) {
		if (strlen(ze) < 1)continue;// no empty line
		if (npuz++ < sgo.vx[0])continue;
		cout << ze << " rank to find sol" << endl;

		if (npuz >= sgo.vx[1])break;
	}
	cout << "end of file   " << endl;
}


// c10 solve quick

void C10() {
	solve.opp = sgo.bfx[8];
	cout << " C10_ solve quick a file of puzzles  "  << endl;
	pzhe = SkbfGetZhePointer();
	char* ze = finput.ze;
	uint32_t npuz = 0;
	while (finput.GetLigne()) {
		if (strlen(ze) < 81)continue;// no empty line
		if (sgo.vx[4]) {
			solve.FilterTridagon(ze);
			continue;
		}
		if (npuz++ < sgo.vx[0])continue;
		if (solve.opp & 1)	cout << ze << " npuz=" << npuz << endl;
		solve.SolveQuick(ze);
		if (npuz >= sgo.vx[1])break;
	}
	cout << "end of file   " << endl;


}


// c11 filter coloin

void C11() {
	solve.opp = sgo.bfx[8];
	cout << " C11_ coloin filter for a file of puzzles  " << endl;
	pzhe = SkbfGetZhePointer();
	char* ze = finput.ze;
	uint32_t npuz = 0;
	while (finput.GetLigne()) {
		if (strlen(ze) < 81)continue;// no empty line
		if (npuz++ < sgo.vx[0])continue;
		cout << ze << " npuz=" << npuz << endl;
		solve.FilterColoin(ze);
		if (npuz >= sgo.vx[1])break;
	}
	cout << "end of file   " << endl;

}

void C12() {
	solve.opp = sgo.bfx[8];
	cout << " C12_ find tridagon digits and boxes  " << endl;
	pzhe = SkbfGetZhePointer();
	char* ze = finput.ze;
	uint32_t npuz = 0;
	while (finput.GetLigne()) {
		if (strlen(ze) < 81)continue;// no empty line
		//if (npuz++ < sgo.vx[0])continue;
		cout << ze << " npuz=" << ++npuz << endl;
		solve.FindDigitsBoxes(ze);
		//if (npuz >= sgo.vx[1])break;
	}
	cout << "end of file   " << endl;

}

// c20 solve to solver base

void C20() {
	solve.opp = sgo.bfx[8];
	cout << " C20_ solve expand puzzles to solver base  " << endl;
	pzhe = SkbfGetZhePointer();
	char* ze = finput.ze;
	uint32_t npuz = 0;
	while (finput.GetLigne()) {
		if (strlen(ze) < 81)continue;// no empty line
		if (solve.opp & 1)	cout << ze << " npuz=" << npuz << endl;
		solve.SolveExpandSolverBase(ze);
	}
	cout << "end of file   " << endl;


}

// c21 test multi floors digits in ze[83]
void C21() {
	solve.opp = sgo.bfx[8];
	cout << " C21_ solve expand puzzles to test multi floors  " << endl;
	pzhe = SkbfGetZhePointer();
	char* ze = finput.ze;
	uint32_t npuz = 0;
	while (finput.GetLigne()) {
		if (strlen(ze) < 84)continue;// mini 2 digits 
		cout << ze << " npuz=" << npuz << endl;
		solve.SolveExpandTestMultiFloors(ze);
	}
	cout << "end of file   " << endl;


}
// c22 solve to more and analysis
void C22() {
	solve.opp = sgo.bfx[8];
	cout << " C22_ solve to more and analysis  " << endl;
	pzhe = SkbfGetZhePointer();
	char* ze = finput.ze;
	uint32_t npuz = 0;
	while (finput.GetLigne()) {
		if (strlen(ze) < 81)continue; 
		cout << ze << " npuz=" << ++npuz << endl;
		solve.SolveExpandMoreAnalysis(ze);
	}
	cout << "end of file   " << endl;


}
// c30 scenario analysis
void C30() {
	solve.opp = sgo.bfx[8];
	cout << " C30_ scenario analysis  " << endl;
	pzhe = SkbfGetZhePointer();
	char* ze = finput.ze;
	finput.GetLigne();
	cout << ze  << endl;
	for (int i = 0; i < 81; i++) {
		if (ze[i] < '1' || ze[i] > '9') ze[i]='.';
	}
	solve.SolveScenario(ze);
}


void C40() {
	solve.opp = sgo.bfx[8];
	cout << " C40_ serate on  a file of puzzles  " << endl;
	ur_ul.Init_urul();
	pzhe = SkbfGetZhePointer();
	char* ze = finput.ze;
	uint32_t npuz = 0;
	while (finput.GetLigne()) {
		if (strlen(ze) < 81)continue;// no empty line
		cout << ze << " npuz=" << ++npuz << endl;
		solve.SolveSerate(ze);
	}
	cout << "end of file   " << endl;


}




//=================== insertions à voir 
/*
*/

/* strtoll example */

/*
#include <stdio.h>      // printf, NULL 
#include <stdlib.h>     // strtoll 

int main()
{
	char szNumbers[] = "1856892505 17b00a12b -01100011010110000010001101100 0x6fffff";
	char* pEnd;
	long long int lli1, lli2, lli3, lli4;
	lli1 = strtoll(szNumbers, &pEnd, 10);
	lli2 = strtoll(pEnd, &pEnd, 16);
	lli3 = strtoll(pEnd, &pEnd, 2);
	lli4 = strtoll(pEnd, NULL, 0);
	printf("The decimal equivalents are: %lld, %lld, %lld and %lld.\n", lli1, lli2, lli3, lli4);
	return 0;
}*/

void Go_0() {

	// open  outputs files 1.txt
	if (sgo.foutput_name) {
		char zn[200];
		strcpy(zn, sgo.foutput_name);
		int ll = (int)strlen(zn);
		strcpy(&zn[ll], "_file1.txt");
		fout1.open(zn);
	}
	{// input file expected
		if (!sgo.finput_name) {
			cerr << "missing input file name" << sgo.finput_name << endl; return;
		}
		finput.open(sgo.finput_name);
		if (!finput.is_open()) {
			cerr << "error open file " << sgo.finput_name << endl;
			return;
		}
	}
	cerr << "running command " << sgo.command << endl;


	switch (sgo.command) {
	//case 0: C0(); break;//  create 512v table
	case 2: C2(); break;// file of sudokus
	case 3:C3(); break;// file of  ranks
	case 10: C10(); break;//solve quick
	case 11: C11(); break;//coloin filter
	case 12: C12(); break;//tridagon digits and boxes
	case 20: C20(); break;//solve expand to solver base
	case 21: C21(); break;//solve expand to test multifllor
	case 22: C22(); break;//solve to more and analysis
	case 30: C30(); break;//scenario analysis
	case 40: C40(); break;//serate clone 

	}
	cerr << "go_0 return" << endl;
}

