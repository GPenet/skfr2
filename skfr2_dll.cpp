
/*
My standard start using maingp as command line analyser
contains the entry G0() called by main
*/
#define _CRT_SECURE_NO_DEPRECATE
//#define _CRT_SECURE_NO_WARNINGS
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
#include "sk0__h.h"
//#include "maingp_user.h"
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
#include "dllusers\u10skfr2s_user.h"
T128 bf128_3x = { BIT_SET_2X,BIT_SET_27 };
SKFR2II skfr2ii;

#include "skfr2a_structs_tables.h"
#include "skfr2a_update_cpp.h"
#include "skfr2a_solv81.h"
#include "skfr2a_structs.h"
#include "skfr2a_slg.h"
#include "skfr2b_solve.h"
#include "skfr2b_minis.h"
#include "skfr2b_sets.h"

//#include "sku02_solveSerate.h"

//=========================== dll to build
/*
get the solution grid to analyze
or get the sukaku status
try to solve send back er ep ed 

*/


//=============== entries   
extern "C" __declspec(dllexport) SKFR2II* Skfr2GetZhePointer();
extern "C" __declspec(dllexport) int Skfr2p(char* puz);

SKFR2II* Skfr2GetZhePointer() {
	pzhe= SkbfGetZhePointer();
	return &skfr2ii;
}


int Skfr2p(char * puz) {
	skfr2ii.puz[81] = skfr2ii.sol[81] = 0;
	memcpy(skfr2ii.puz, puz, 81);
	memcpy(skfr2ii.sol, puz, 81);
	skfr2ii.Er =  skfr2ii.EP =skfr2ii.ED = 0;
	//if(!SkbfCheckValidityQuick(puz)) return 1;// not valid
	solve.SolveSerate(puz);
	return (skfr2ii.Er>0);
}



