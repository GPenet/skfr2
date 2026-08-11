#pragma once


struct SOLVE;
//========================================= Mini struct
struct MINIS {
	BF128 sold[9]; //sol per digit
	BF128 unsolved_cells;
	int sol[81];
	// digit active minir minic
	uint32_t damr[9], damc[9];
	void InitSolPerDigit(int* sol);
	int Build(SOLVE& o);
	void DumpSolsPerDigit1();
	void Dump1();
};
//================================= sets struct
struct DSETS {
	BF128 rcb[27],// 27 sets 9 digits (some empty)
		d234m; // sets active by size 2 3 4 more
	void Build(int dig, BF128& o);
};
struct SETS {
	SOLV81 infield;
	DSETS ds[9]; //9 digits
	BF128 c2345[4]; // active cells by size
	SOLV81 * sv;
	void Build(SOLVE& o);
};
//=================================================== solve 
struct SOLVE {
	BF128 rclean1[9];//solvf1  back from slg in serate mode 
	uint32_t opp; //print control
	int g0[81]; // solution given by check brute force
	char puz[82]; // give puzzle 
	int step;
	SOLV81 sv81w ,sv81step[10];
	MINIS minis;
	SETS sets;
	SOLVE() {		BuildTrcb(); BuildSeencols();}
	int Init(char* ze);
	void Clean(int dig, BF128& c);
	void CleanCell(int cell,int digs);
	int SinglesAtStart();
	int  Backdoor1();
	uint32_t  Backdoor2();
	int SolveMinis();
	int IsTridagon();
	void SetsBuild();
	int SolveUnits2c();
	int SolveUnits2h();
	int SolveUnits3c();
	int SolveUnits3h();
	int SolveF1(int opt=0,int modeserate=0);// 1 si rc
	int SolveU1(int modeserate=0);
	int SolveU1SC();
	int SolveUR();
	int SolveUL();
	void SolveUR_b();
	void SolveUR4(int c1,int c2,int c3,int c4);
	int IsYbiv(int serate = 0);
	int IsBandStack();
	int IsBandStack23(int serate = 0);
	int IsAllBiv(int serate = 0);
	void SolveSerate(char* ze);
}solve;
