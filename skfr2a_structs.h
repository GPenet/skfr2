#pragma once
/*
dm is a classical map as in the brute force 3 bands per digits
crm is a cell/row/digit map
 one row per crm
 each row 9 cells (3x3)
 each cell 9 digits
*/
char cout1[6], cout2[6], cout3[6];// area to print cands
class CAND {
	uint16_t dc; // digit,cell
public:
	inline void Set(UCHAR d, UCHAR c) {
		register uint16_t x = c;
		x <<= 8; x |= d; dc = x;
	}
	inline void SetDc(uint16_t dce) {
		dc = dce;
	}
	inline void Set(uint16_t d, uint16_t c) {
		register uint16_t x = c;
		x <<= 8; x |= d; dc = x;
	}
	inline void Set(int d, int c) {
		register uint16_t x = c;
		x <<= 8; x |= d; dc = x;
	}
	inline void Set(uint32_t d, uint32_t c) {
		register uint16_t x = (uint16_t)c;
		x <<= 8; x |= (uint16_t)d; dc = x;
	}
	inline uint32_t Digit() { return dc & 0xff; }
	inline uint32_t Cell() { return (dc >> 8) & 0xff; }
	inline uint16_t GetV(){ return dc; }
	char* Out(char* ws) {// ws must be length >=6
		ws[5] = 0;
		ws[0] = (char)(Digit() + '1');
		memcpy(&ws[1], cell_names[Cell()], 4);
		return ws;
	}
};
/* class BIVCANDS
this is a bi-value shown as 2 CAND. Owner knows what are first an second cand
The bivalue is stored in a 32 bit integer
*/
class BIVCANDS {
	uint32_t v;
public:
	inline void Set(int d1, int c1, int d2, int c2) {
		register uint32_t x = c1;
		x <<= 8; x |= d1; v = x;
		x = c2;	x <<= 8; x |= d2; v |= x<<16;
	}
	inline void Get(int & d1, int & c1, int& d2, int & c2) {
		register uint32_t x = v;
		d1 = x & 0xff; x >>= 8;c1= x & 0xff; x >>= 8;
		d2 = x & 0xff; x >>= 8; c2 = x & 0xff;
	}
	inline void Set(CAND a, CAND b) {
		v = (uint32_t)a.GetV() | ((uint32_t)b.GetV() << 16);
	}
	inline CAND GetA() { CAND w; w.SetDc(v & 0xff); return w; }
	inline CAND GetB() { CAND w; w.SetDc(v >>16); return w; }

};
class DM9 {// pm to store a property
	BF128 dm[9];
public:
	inline void Init() { memset(dm, 0, sizeof dm); }
	inline void InitAll() { memset(dm, 255, sizeof dm); }
	inline void Add(CAND& cd) {
		dm[cd.Digit()].Set_c(cd.Cell());
	}
	inline void Set(int d, int c) { dm[d].Set_c(c); }
	inline int On(int d, int c) { return dm[d].On_c(c); }
	inline int Off(int d, int c) { return dm[d].Off_c(c); }
	inline int On(CAND& cd) {
		return dm[cd.Digit()].On_c(cd.Cell());
	}
	inline int Off(CAND& cd) {
		return dm[cd.Digit()].Off_c(cd.Cell());
	}
	void Assign(CAND& cd) {
		int d = cd.Digit(), c = cd.Cell();
	}
	inline BF128 Getd(int d) { return dm[d]; }
	void Orx(DM9& o) {
		for (int i = 0; i < 9; i++)dm[i] |= o.dm[i];
	}
	inline void Ordx(int d, BF128& x) {
		dm[d] |= x;
	}
	void Andx(DM9& o) {
		for (int i = 0; i < 9; i++)dm[i] &= o.dm[i];
	}
	int IsEmpty() {
		BF128 w = dm[0];
		for (int i = 1; i < 9; i++)w |= dm[i];
		return w.isEmpty();
	}

};

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
