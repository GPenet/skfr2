#pragma once


//========================================= solve status
struct SOLV81 {
	BF128 unsolved_cells,unsdiag,
		dm[9],// digits maps
		ccm[9]; // cells map for count 'n'
	BF128 dmd[9]; // diag view 
	int* g0; // solution given by check brute force
	uint32_t cells[81]; // digits status
	int loop,bug;
	//============ singles to the end and back doors
	void Init(int* g);
	void Idiag() {
		unsdiag.Diag3x27(unsolved_cells);
		for (int i = 0; i < 9; i++)
			dmd[i].Diag3x27(dm[i]);
	}
	void InitSlg(int* g);
	int Assign(int dig, int cell);
	int Clear(int dig, int cell);
	void Clean(int dig, BF128& c);
	void CleanDiag(int dig, BF128& c);
	inline void CleanBand(int d, int ib, int b) {
		BF128 w; w.SetAll_0();
		w.bf.u32[ib] = b;
		Clean(d, w);
	}
	inline void CleanBandDiag(int d, int ib, int b) {
		BF128 w,x; w.SetAll_0();
		w.bf.u32[ib] = b;
		x.Diag3x27(w);
		Clean(d, x);
	}	
	void CleanCell(int cell, int digs);
	int GetUnitFloors(int iu, int n);
	int  GoSudoku(char* puz);
	int  DoSingles();
	void DoLastInCell();
	void DoLastInUnit();
	int DoEr10LastInUnit();
	int DoEr12LastInBox();
	int DoEr15LastInRC();
	int DoEr17();// locked in box  clearing row/col 
	int DoEr20();// hidden pair, hidden fix
	int DoEr23();// single in cell 
	int DoEr25();// Hidden triplet, fix 
	int DoDig_R_C_Fix(BF128 f, int id, int ib,int rc, int diag);
	int DoEr26_28();// locked in box, RC 
	int DoEr26();// locked in box, RC 
	int DoEr28();// locked in box, RC 
	int DoEr30U(int iu);// one unit NakedPair
	int DoEr30();// NakedPair  
	int DoEr32D(int d,int diag); // one digit
	int DoEr32();// XWing 
	int DoEr34U(int iu);// HiddenPair 
	int DoEr34();// HiddenPair 
	int DoEr36U(int iu);// one unit Naked_triplet
	int DoEr36();// Naked_triplet 
	int DoEr38D(int d, int diag); // one digit
	int DoEr38();// swordfish 
	int DoEr40U(int iu);// one unit hidden_triplet
	int DoEr40();// HiddenTriplet 
	int DoEr42();// XY Wing  
	int DoEr44BS(int ibs);// band XYZ Wing  
	int DoEr44();// XYZ Wing  
	int DoEr50U(int iu);// one unit Naked_quad
	int DoEr50();// Naked_quad 
	int DoEr52D(int d, int diag); // one digit
	int DoEr52();// Jellyfish 
	int DoEr54U(int iu);// one unit hidden_quad
	int DoEr54();// HiddenQuad  
	int BugCheck(BF128& m, int v);
	int BugNakedPair(BF128& m, int unit,int v);
	int BugNakedTriplet(BF128& m, int unit, int v);
	int BugNakedTripletB(BF128& m, int unit, int v);
	int BugNakedQuad(BF128& m, int unit, int v);
	int BugNakedQuadB(BF128& m, int unit, int v);
	int BugNakedQuadC(BF128& m, int unit, int v);
	int BugNaked5A(BF128& m, int unit, int v);
	int BugNaked5B(BF128& m, int unit, int v);
	int DoEr56();// BUG s 56 to 61
	int DoEr62();//aligned pair	
	int Er6x();
	int DoEr6xD(int d, int r);// one digit with elims 
	int DoEr6x(int rat);// X cycle X chains
	int DoEr6ycy(int rat);// y cycle  
	int DoEr6ych(int rat);// y chains  
	int DoEr70();// init xy chains  and look for ER70
	int DoEr7x(int rat );//  xy chains  >70

	int DoEr75();//aligned triplet
	int DoEr75X();//Nishio

	int DoEr80();//multichains
/*
AIC_X_cycle=65,
	Forcing_ChainX=66,          ///<  at least 6.6 6.7 f(length)
	AIC_XY=70,
	AlignedTripletExclusion=75 ,
	NishioForcingChain=75 ,
	MultipleForcingChain=80,
*/
	int  DoSinglesTE();
	int DoLastInCellTE();
	int DoLastInUnitTE();
	
	//==== multi floors
	int DoLastInUnitMF(int f); 
	int GetMinMF(int& f, int& unit, int& digit);
	// check if valid
	int Conflict() {
		for(int i=0;i<81;i++)	if (!cells[i]) {
				cout << " cell empty " << i+1 << endl;
				return 1;

			}
		for (int iu = 0; iu < 27; iu++) {
			BF128 wu = units3xBM[iu];
			for (int id = 0; id < 9; id++) {
				if ((wu & dm[id]).isEmpty()) {
					cout <<"unit "<<iu+1 <<" empty for digit " << id+1 << endl;
					return 1;
				}
			}
		}
		return 0;
	}
#ifdef SEROUT

	void ImageCandidats(int known = 1);
	void ImageCandidatsShort();
	void ImageOne(int dig, int known = 1);
	void ImageMulti(int digs, int dummy, int known = 1);
#endif

}sW;
