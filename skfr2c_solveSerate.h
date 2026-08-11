/*

enum SolvingTechnique {
	LastCell = 10,				///< last cell in row column box
	SingleBox = 12,               ///< single in box
	Single_R_C = 15,              ///< single in row or column
	Single_after_Locked = 17,		///< locked in box  clearing row/col ?? giving a fix??
	PointingClaiming = 19,        ///< unknown
	HiddenPair_single = 20,		///< hidden pair, hidden fix 
	NakedSingle = 23,				///< cell one candidate 
	HiddenTriplet_single = 25,    ///< Hidden triplet, fix
	Locked_box = 26,				///< locked in box, no fix
	Locked_RC = 28,				///< locked in row/col  no fix
	NakedPair = 30,               ///< 2 cells containing 2 digits
	XWing = 32,                   ///< XWing
	HiddenPair = 34,              ///< 2 digits locked in 2 cell
	Naked_triplet = 36,           ///< 3 cells containing 3 digits   
	swordfish = 38,               ///< swordfish   
	HiddenTriplet = 40,           ///< 3 digits in 3 cells
	XYWing = 42,					///< XYWing
	XYZWing = 44,					///< XYZWing
	UniqueRect1 = 45,             ///< UR type(s)  basic, on digit active, twins
	UniqueRect2 = 46,             ///< UR hidden locked setlocked set
	UniqueRect3 = 47,             ///< UR naked locked set
	UniqueLoop1 = 48,             ///< UL  locked set also URnaked quad
	UniqueLoop2 = 49,             ///< UL naked locked set
	NakedQuad = 50,				///< 4 cells with 4 digits
	UniqueLoop3 = 51,             ///< UL highest rating
	Jellyfish = 52,				///< jellyfish
	HiddenQuad = 54,				///< 4 digits in 4 cells
	BUG = 56,                     ///< in fact 5.6 to 6.1
	AlignedPairExclusion = 62,
	AIC_X_cycle = 65,
	Forcing_ChainX = 66,          ///<  at least 6.6 6.7 f(length)
	AIC_XY = 70,
	AlignedTripletExclusion = 75,
	NishioForcingChain = 75,
	MultipleForcingChain = 80,
	DynamicForcingChain = 85,
	DynamicForcingChainPlus = 90,
	NestedForcingChain = 95,
	NestedLevel3 = 100,
	NestedLevel4 = 105,
	NesttedLevel5 = 110

};

void PUZZLE::SetEr() {  // something found at the last difficulty level
	if((cycle==1)&& difficulty>edmax) edmax=difficulty;
	if(((!assigned)|| (!epmax))&& difficulty>epmax) epmax=difficulty;
	if(difficulty>ermax) ermax=difficulty;
}
*/


/*	Upper bound for chains is actually unbounded: 
the longer chain, the higher rating.
 */
USHORT  steps[] = { 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128,
			192, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 6144, 8192 };

struct TWASS {// temp storage cells to assign
	int t[20][2], nt;
	void Add(int c, int d) {
		if (nt > 19) return;
		for (int i = 0; i < nt; i++) if (c == t[i][0]) return; // no redundancy
		t[nt][0] = c; t[nt++][1] = d;
	}
	void AddMess(int c, int d) {
		cout << "asked ass c " << c + 1 << " d " << d + 1 << endl;
		Add(c, d);
	}	
	void DoAss();
}twass;
void TWASS:: DoAss() {
	for (int i = 0; i < nt; i++)
		solve.sv81w.Assign(t[i][1], t[i][0]);
}
struct URUL {
	int c3, c4, digs, ncells,rating;
	inline void Add(int ec3, int ec4, int d, int n=4, int r = 0) {
		c3 = ec3; c4 = ec4; digs = d; ncells = n; rating = r;
	}


}turul[20];

struct SERATE {
	SOLV81 svr,*mysv;
	BF128 cbiv;
	int er, ep, ed, miner, minep, mined, maxer, maxep, maxed,assigned;
	int stop,activeunits,activedigits,
		isybiv,isbs23,isallbiv,nturul;
	void Init(SOLVE * s) {	er = ep = ed =assigned=stop= 0;	
	mysv = &s->sv81w;
	svr = *mysv;
	}
	void SetRating(int x) {
#ifdef SEROUT
		cout << "tt-> active Er " << x  << endl;
		if (x >= SEROUT && x <= SEROUTEND) mysv->ImageCandidatsShort();
#endif
		if (!ed)ep =ed = x;
		if (x <= 23)assigned++;
		if ((!assigned) && ep<x  ) ep = x;
		if (x > er) er = x;
	}
}serate;

#include "skfr2d_solveSerate_url.h"
#include "skfr2d_solveSerate_bugs.h" 
#include "skfr2d_solveSerate_exclusion.h" 
#include "skfr2d_solveSerate_xchain.h" 
#include "skfr2d_solveSerate_ychain.h" 
#include "skfr2d_xybiv.h"
#include "skfr2d_xybiv_multi.h"


void SOLVE::SolveSerate(char* ze) {
	skfr2ii.Er = skfr2ii.EP = skfr2ii.ED = 200;
	if (Init(ze))return;
	skfr2ii.Er = skfr2ii.EP = skfr2ii.ED = 201;
	ur_ul.Init_urul();
#ifdef SEROUT
	for (int i = 0; i < 81; i++) cout << pzhe->gsol[i] + 1;
	cout << " sol returned" << endl;
	sv81w.ImageCandidatsShort();
#endif
	step = 0;
	serate.Init(this);

	skfr2ii.Er =  skfr2ii.EP = skfr2ii.ED = 202;
//	return;
	while (++step <50 && sv81w.unsolved_cells.isNotEmpty()) {
		if (serate.stop || sv81w.bug) break;
		sv81w.Idiag();
		{
			if (sv81w.DoEr10LastInUnit()) continue;
			if (sv81w.DoEr12LastInBox()) continue;
			if (sv81w.DoEr15LastInRC()) continue;
			if (sv81w.DoEr17()) continue;
			if (sv81w.DoEr20()) continue;
			if (sv81w.DoEr23()) continue;
			if (sv81w.DoEr25()) continue;
			if (sv81w.DoEr26()) continue;
			if (sv81w.DoEr28()) continue;
		}
		// get active units
		{
			SetsBuild();
			serate.activeunits = SolveU1(1);
#ifdef SEROUT
	cout << Char27out(serate.activeunits) << " active units" << endl;
#endif			
		}
		if (sv81w.DoEr30()) continue;

		// get active digits
		{
			serate.activedigits = SolveF1(1, 1);// rc
			//cout << Char9out(serate.activedigits) << " active digits" << endl;
			//for (int i = 0; i < 9; i++)
			//	if (serate.activedigits & (1 << i))
			//		sv81w.ImageOne(i);
#ifdef SEROUT
			cout << Char9out(serate.activedigits) << " active digits" << endl;
#endif			

		}
		{
			if (sv81w.DoEr32()) continue;
			if (sv81w.DoEr34()) continue;
			if (serate.activeunits)	if (sv81w.DoEr36()) continue;
			if (sv81w.DoEr36()) continue;
			if (sv81w.DoEr38()) continue;
			if (sv81w.DoEr40()) continue;

		}
#ifdef SEROUT
		sv81w.ImageCandidatsShort();
#endif		// 42 44   
		{
			serate.isybiv = IsYbiv(1);
#ifdef SEROUT
			cout << serate.isybiv << " active ybiv" << endl;
#endif	
			if (serate.isybiv && sv81w.DoEr42()) continue;
			if (sv81w.DoEr44()) continue;
		}
		//UR UL + 52 54
		{
			ur_ul.Build_uruld();
			//sv81w.ImageCandidatsShort();
			if (ur_ul.DoEr45()) continue;
			if (ur_ul.DoEr46()) continue;
			if (ur_ul.DoEr47()) continue;
			if (ur_ul.DoEr48()) continue;
			if (ur_ul.DoEr49()) continue;
			if (serate.activeunits && sv81w.DoEr50()) continue;
			if (ur_ul.DoEr50()) continue;
			if (ur_ul.DoEr51()) continue;
			if (serate.activedigits && sv81w.DoEr52()) continue;
			if (ur_ul.DoEr52()) continue;
			if (ur_ul.DoEr53()) continue;
			if (serate.activeunits && sv81w.DoEr54()) continue;
		}
		if (sv81w.DoEr56()) continue;
		//serate.isbs23 = IsBandStack23(1);
		if (sv81w.DoEr62()) continue;
		break;
#ifdef SEROUT
		sv81w.ImageCandidatsShort();
#endif
		{
			//cout << "entry 65/70" << endl;
			if (serate.activedigits && sv81w.DoEr6x(65)) continue;
			if (serate.activedigits && sv81w.DoEr6x(66)) continue;
			if (serate.isybiv && sv81w.DoEr6ycy(66)) continue;
			if (serate.activedigits && sv81w.DoEr6x(67)) continue;
			if (serate.isybiv && sv81w.DoEr6ycy(67)) continue;
			//if (serate.isybiv && sv81w.DoEr6ych(67)) continue;
			if (serate.activedigits && sv81w.DoEr6x(68)) continue;
			if (serate.isybiv && sv81w.DoEr6ycy(68)) continue;
			if (serate.isybiv && sv81w.DoEr6ych(68)) continue;
			if (serate.activedigits && sv81w.DoEr6x(69)) continue;
			if (serate.isybiv && sv81w.DoEr6ycy(69)) continue;
			if (serate.isybiv && sv81w.DoEr6ych(69)) continue;
		}
		if (0) {
			if (sv81w.DoEr70()) continue;
			if (sv81w.DoEr7x(71)) continue;
			if (sv81w.DoEr75()) continue;// triplet
			if (serate.activedigits && sv81w.DoEr75X()) continue;
			if (sv81w.DoEr80()) continue;// multi
		}
		if (serate.activedigits && sv81w.DoEr75X()) continue;

		//sv81w.ImageCandidatsShort();
		/*


		//______________ Band/Stack elims
		if (IsBandStack23()) {
			if (opp & 1)cout << " band/stack  active " << endl;
			continue;
		}

		//______________ xy chains  elims
		if (IsAllBiv()) {
			if (opp & 1)cout << " xy chains  active " << endl;
			continue;
		}


			*/
		break; // not solved 
	}
	skfr2ii.Er = serate.er; skfr2ii.EP = serate.ep; skfr2ii.ED = serate.ed;
	if(sv81w.unsolved_cells.isNotEmpty()) 		skfr2ii.Er += 1000;	
	//else  {}


}
int SOLV81::GetUnitFloors(int iu, int n) {
	BF128 w = unsolved_cells & units3xBM[iu];
	int f = 0;
	for (int id = 0; id < 9; id++) {// 9 digits
		int nn = (w & dm[id]).Count96();
		if (nn && nn <= n) f |= 1 << id;
	}
	return f;
}


int SOLV81::DoEr10LastInUnit() {
	int iret = 0;
	for (int iu = 0; iu < 27; iu++) {
		BF128 w = unsolved_cells & units3xBM[iu];
		if (w.isEmpty())continue;
		if (w.Count() == 1) {// last cell in unit
			int cell = w.getFirstCell(), dig;
			bitscanforward(dig, cells[cell]);// get the dig
			Assign(dig, cell);			serate.SetRating(10);
			iret++;		
		}
	}
	return iret;
}
int SOLV81::DoEr12LastInBox() {
	int iret = 0;
	for (int iu = 18; iu < 27; iu++) {
		BF128 w = unsolved_cells & units3xBM[iu];
		if (w.isEmpty())continue;
		for (int id = 0; id < 9; id++) {
			BF128 wd = w & dm[id];
			if (wd.Count() == 1) {//digit  last cell in unit
				int cell = wd.getFirstCell();
				Assign(id, cell);			serate.SetRating(12);
				iret++;		w &= unsolved_cells; // update w
			}
		}
	}
	return iret;
}
int SOLV81::DoEr15LastInRC() {
	int iret = 0;
	for (int iu = 0; iu < 18; iu++) {
		BF128 w = unsolved_cells & units3xBM[iu];
		if (w.isEmpty())continue;
		for (int id = 0; id < 9; id++) {
			BF128 wd = w & dm[id];
			if (wd.Count() == 1) {//digit  last cell in unit
				int cell = wd.getFirstCell();
				if (solve.opp & 2)cout << "  last in RC digit " << id + 1 << " c " << cell + 1 << " unit " << iu + 1 << endl;
				Assign(id, cell);			serate.SetRating(15);
				iret++;		w &= unsolved_cells; // update w
			}
		}
	}
	return iret;
}
// derived from the brute force
int SOLV81::DoEr17() { // locked in box  clearing row/col -> giving a fix
	twass.nt = 0;
	int b2[3] = { 1,0,0 }, b3[3] = { 2,2,1 };
	BF128 unsdiag;
	unsdiag.Diag3x27(unsolved_cells);
	for (int id = 0; id < 9; id++) {// 9 digits
		//cout << "17 digit " << id + 1 << endl;
		BF128 w = dm[id], wdiag; 
		int cell;
		if (w.Count96() <= 12) continue;
		for (int ib = 0; ib < 3; ib++) {// 3 bands
			register uint32_t A = w.bf.u32[ib],Ar=A,
				Shrink= (TSM[A & X9] | TSM[(A >> 9) & X9] << 3 
					| TSM[(A >> 18)] << 6);
			A &= TCM[Shrink];// if 0 bug not expected here
			if (Ar == A) continue;// nothing expected here
			A &= unsolved_cells.bf.u32[ib];
			// check  assigned rows boxes
			for (int iw = 0; iw < 6; iw++) {
				register uint32_t R = A & t6RB[iw];
				if (_popcnt32(R) == 1) {
					bitscanforward(cell, R);
					twass.Add(cell + 27 * ib, id);
				}
			}		
		}
		wdiag.Diag3x27(w);		// same on column after tranpose
		for (int ib = 0; ib < 3; ib++) {// 3 bands
			register uint32_t A = wdiag.bf.u32[ib], Ar = A,
				Shrink = (TSM[A & X9] | TSM[(A >> 9) & X9] << 3
					| TSM[(A >> 18)] << 6);
			A &= TCM[Shrink];// if 0 bug not expected here
			if (Ar == A) continue;// nothing expected here
			A &= unsdiag.bf.u32[ib];
			// check  assigned rows boxes
			for (int iw = 0; iw < 6; iw++) {
				register uint32_t R = A & t6RB[iw];
				if (_popcnt32(R) == 1) {
					bitscanforward(cell, R);
					twass.Add(cdiag[cell + 27 * ib], id);
				}
			}
		}
	}
	if (twass.nt) {
		serate.SetRating(17);
		twass.DoAss();		return twass.nt;	}
	else return 0;
}
int SOLV81::DoEr20() {// hidden pair, hidden fix
	twass.nt = 0;
	for (int iu = 0; iu < 27; iu++) {// 27 units
		int f = GetUnitFloors(iu, 2);
		BF128 w = unsolved_cells & units3xBM[iu];
		for (int ifl = 0; ifl < 36; ifl++) {
			int fl = floors_2d[ifl];
			if ((fl & f) != fl) continue; // no hidden pair
			int d1, d2;
			bitscanforward(d1, fl);
			bitscanreverse(d2,fl);
			BF128 wd = w & (dm[d1] | dm[d2]);
			if (wd.Count96() != 2) continue; 
			// hidden pair iu,d1,d2 look for singles
			for (int id = 0; id < 9; id++) {
				BF128 wd2 = (w & dm[id]) - wd;
				if (wd2.Count96() == 1) {// single to assign
					register int cell = wd2.getFirstCell();
					twass.Add(cell, id);
				}
			}
		}

	}
	if (twass.nt) {
		serate.SetRating(20);
		twass.DoAss();		return twass.nt;
	}
	else return 0;
}
int SOLV81::DoEr23() {// single in cell
	twass.nt = 0;
	BF128 w = unsolved_cells & ccm[0];
	//ImageCandidatsShort();
	//char ws[82]; ws[81] = 0;
	//cout << w.String3X(ws) << " singles in cell" << endl;
	if (w.isEmpty())return 0;
	int tc[81], ntc = w.Table3X27(tc);

	for (int i = 0; i < ntc; i++) {
		int cell = tc[i], dig;
		bitscanforward(dig, cells[cell]);// get the dig
		twass.Add(cell, dig);
	}
	if (twass.nt) {
		serate.SetRating(23);
		twass.DoAss();		return twass.nt;
	}
	else return 0;
}
int SOLV81::DoEr25() { // Hidden triplet, fix
	twass.nt = 0;
	char ws[82]; ws[81] = 0;
	//ImageCandidatsShort();
	for (int iu = 0; iu < 27; iu++) {// 27 units
		int f = GetUnitFloors(iu, 3);
		BF128 w = unsolved_cells & units3xBM[iu];
		for (int ifl = 0; ifl < 84; ifl++) {
			int fl = floors_3d[ifl];
			if ((fl & f) != fl) continue; // no hidden triplet
			int d1, d2,d3;
			bitscanforward(d1, fl); fl ^= 1 << d1;
			bitscanreverse(d2, fl);
			bitscanforward(d3, fl);			
			BF128 wd = w & ((dm[d1] | dm[d2]) | dm[d3]);
			if (wd.Count96() != 3) continue;
			// hidden pair iu,d1,d2 look for singles
			for (int id = 0; id < 9; id++) {
				BF128 wd2 = (w & dm[id]) - wd;
				if (wd2.Count96() == 1) {// single to assign
					register int cell = wd2.getFirstCell();
					twass.Add(cell, id);
				}
			}
		}
	}
	if (twass.nt) {
		serate.SetRating(25);
		twass.DoAss();		return twass.nt;
	}
	else return 0;	return 0;
}
int t_row_imini[9] = {
	0770,0707,077,0770000,0707000,077000,
	0770000000,0707000000,077000000 };
int t_box_imini[9] = {
	07007000,070070000,0700700000,
	07000007,070000070,0700000700,
	07007,070070,0700700 };
int SOLV81::DoDig_R_C_Fix(BF128 w, int id, int ib,int rc, int diag) {
	int iret = 0;
	register uint32_t A = w.bf.u32[ib], Ar = A,
		Sh  = (TSM[A & X9] | TSM[(A >> 9) & X9] << 3
			| TSM[(A >> 18)] << 6);
	A &= TCM[Sh ];// if 0 bug not expected here
	if (!A) { 
		cout << Char27out(Ar) << "bug DoEr17" << endl; serate.stop = 1; return 0;
	}
	if (Ar == A) return 0;;// nothing expected here
	//get single mini in row and boxes
	// p3r_32[9] = 	07,   070,    0700,
	// p3b_32[9] =  0111, 0222, 0444,
	int rs = 0, bs = 0, imini;
	for (int i = 0; i < 3; i++) {
		register int r = Sh & p3r_32[i], x = Sh & p3b_32[i];
		if (_popcnt32(r) == 1) rs |= r;
		if (_popcnt32(x) == 1) bs |= x;
	}
	if(rc&1){// box single not row single rows cleaned
		register int bss = bs & ~rs;
		if (bss) 	for (int i = 0; i < 3; i++) {
			register int bssr = bss & p3r_32[i];
			if (bssr) {
				bitscanforward(imini, bssr);
				register int K = Ar & t_row_imini[imini];
				if(diag)CleanBandDiag(id, ib, K);
				else CleanBand(id, ib, K);
				serate.SetRating(26);iret++;	}
		}		
	}
	if (rc & 2) {// rc single not box single box cleaned
		register int rss = rs & ~bs;
		if (rss) 	for (int i = 0; i < 3; i++) {
			register int rssr = rss & p3b_32[i];
			if (rssr) {
				bitscanforward(imini, rssr);
				register int K = Ar & t_box_imini[imini];
				if (diag)CleanBandDiag(id, ib, K);
				else CleanBand(id, ib, K);
				serate.SetRating(28);	iret++;	}
		}		
	}	
	return iret;
}
int SOLV81::DoEr26 () {// locked in box, no fix
	int iret = 0;
	char ws[82]; ws[81] = 0;
	int b2[3] = { 1,0,0 }, b3[3] = { 2,2,1 };
	//ImageCandidatsShort();
	for (int id = 0; id < 9; id++) {// 9 digits
		BF128 w = dm[id], wdiag = dmd[id];
		if (w.Count96() <= 12) continue;
		for (int ib = 0; ib < 3; ib++) // 3 bands
			iret += DoDig_R_C_Fix(w, id, ib, 1, 0);
		for (int ib = 0; ib < 3; ib++) // 3 bands
			iret += DoDig_R_C_Fix(wdiag, id, ib, 1, 1);
	}
	return iret;
}
int SOLV81::DoEr28() {// locked in box, no fix
	int iret = 0;
	char ws[82]; ws[81] = 0;
	int b2[3] = { 1,0,0 }, b3[3] = { 2,2,1 };
	//ImageCandidatsShort();
	for (int id = 0; id < 9; id++) {// 9 digits
		BF128 w = dm[id], wdiag = dmd[id];
		if (w.Count96() <= 12) continue;
		for (int ib = 0; ib < 3; ib++) // 3 bands
			iret += DoDig_R_C_Fix(w, id, ib, 2, 0);
		for (int ib = 0; ib < 3; ib++) // 3 bands
			iret += DoDig_R_C_Fix(wdiag, id, ib, 2, 1);
	}
	return iret;
}
int SOLV81::DoEr26_28() {// locked in box, no fix
	int iret= 0;
	char ws[82]; ws[81] = 0;
	int b2[3] = { 1,0,0 }, b3[3] = { 2,2,1 };
	//ImageCandidatsShort();
	for (int id = 0; id < 9; id++) {// 9 digits
		BF128 w = dm[id], wdiag=dmd[id];
		if (w.Count96() <= 12) continue;
		for (int ib = 0; ib < 3; ib++) // 3 bands
			iret += DoDig_R_C_Fix(w, id, ib, 1, 0);
		for (int ib = 0; ib < 3; ib++) // 3 bands
			iret += DoDig_R_C_Fix(wdiag, id, ib, 1, 1);
	}



	return iret;
}
int SOLV81::DoEr30U(int iu) {
	uint32_t cleandone = 0;
	uint32_t bitiu = 1 << iu;
	BF128 uc = unsolved_cells & units3xBM[iu];
	BF128 uc2 = uc & solve.sets.c2345[0];
	if (uc2.Count96() < 2) return 0;
	while (1) {
		int c1 = uc2.getFirstCell(), c1v = cells[c1];
		uc2.Clear_c(c1);
		BF128 uc2b = uc2;
		int tc[10], ntc = uc2b.Table3X27(tc);
		for (int ic2 = 0; ic2 < ntc; ic2++) {
			int c2 = tc[ic2], c2v = cells[c2];
			if (c2v != c1v) continue;
			uc2.Clear_c(c2);// forget it now
			int d1, d2;
			bitscanforward(d1, c1v);
			bitscanreverse(d2, c1v);
			DSETS& ds1 = solve.sets.ds[d1], & ds2 = solve.sets.ds[d2];
			uint32_t uds1 = ds1.d234m.bf.u32[0],
				uds2 = ds2.d234m.bf.u32[0], udsc = uds1 & uds2;
			if (udsc & bitiu) 	continue;// no clearing
			BF128 cx; cx.SetAll_0(); cx.Set_c(c1); cx.Set_c(c2);
			BF128 clx = ds1.rcb[iu] - cx;
			if (clx.isNotEmpty()) {
				cleandone = 1;
				Clean(d1, clx);
			}
			clx = ds2.rcb[iu] - cx;
			if (clx.isNotEmpty()) {
				cleandone = 1;
				Clean(d2, clx);
			}
		}
		if (uc2.Count96() < 2) break;
	}
	//if (cleandone) cout << iu + 1 << " unite active naked pair" << endl;
	return  cleandone;
}
int SOLV81::DoEr30() {// NakedPair
	//ImageCandidatsShort();
	int iret = 0;	
	for (int iu = 0; iu < 27; iu++) if(serate.activeunits & (1<<iu))
		iret += DoEr30U(iu);
	if (iret) 		serate.SetRating(30);
	return iret;
}
int SOLV81::DoEr32D(int d, int diag) {
	int tri[9], tr[9], ntr = 0,iret=0;
	BF128 w;
	if (diag) w = dmd[d] & unsdiag;
	else w= dm[d] & unsolved_cells;
	for (int i = 0; i < 9; i++)	if ((w & units3xBM[i]).Count96() == 2) {
		int ib = i / 3, ir = i - 3 * ib;
		tri[ntr] = i;
		tr[ntr++] = (w.bf.u32[ib] >> 9 * ir) & 0x1ff;// row
	}
	if (ntr < 2) return 0;
	for (int i = 0; i < ntr-1; i++) {
		register int a = tr[i];
		for (int j = i + 1; j < ntr; j++) {
			register int b = tr[j];
			if (a != b) continue;
			int c1, c2;
			bitscanforward(c1, a);
			bitscanreverse(c2, a);
			BF128 wr = units3xBM[tri[i]]; wr |= units3xBM[tri[j]];
			BF128 wc = units3xBM[c1 + 9]; wc |= units3xBM[c2 + 9];
			wr &= w; wc &= w;	if (wr != wc) {
				//cout<<diag << "active " << tri[i] << tri[j] << c1 << c2 << endl;
				wc -= wr;
				if (diag)CleanDiag(d, wc);
				else Clean(d, wc);
				iret++;
			}
		}
	}
	return iret;
}
int SOLV81::DoEr32() {// XWing
	int iret = 0;
	for (int id = 0; id < 9; id++) {// 9 digits
		if (!(serate.activedigits & (1 << id))) continue;
		//cout << "32 digit " << id + 1 << endl;
		iret += DoEr32D(id, 0) | DoEr32D(id, 1);

	}
	if (iret) serate.SetRating(32);
	return iret;
}
int SOLV81::DoEr34U(int iu) {
	//cout << "34 try unit " << iu << endl;
	int tdi[9], ntdi = 0, iret = 0;
	BF128 w= unsolved_cells& units3xBM[iu];
	if (w.Count96() < 3) return 0;// should never be
	for (int i = 0; i < 9; i++)
		if ((w & dm[i]).Count96() == 2) tdi[ntdi++] = i;
	//cout << "ntdi=" << ntdi<<" ";
	//for (int i = 0; i < ntdi; i++) cout << tdi[i] + 1; 
	//cout  << endl;
	if (ntdi < 2) return 0;// should never be
	for (int i = 0; i < ntdi - 1; i++) {
		register int di = tdi[i];
		BF128 wi = w & dm[di];
		for (int j = i + 1; j < ntdi; j++) {
			register int dj = tdi[j];
			BF128 wj = w & dm[dj];
			if (wi == wj) {// hidden pair?? if clean
				for (int id = 0; id < 9; id++) {
					if (id == di || id == dj) continue;
					BF128 wc = wi & dm[id];
					if (wc.isNotEmpty()) {
						Clean(id, wc);
						iret++;
					}
				}
			}
		}
	}
	return iret;
}
int SOLV81::DoEr34() {// HiddenPair
	//ImageCandidatsShort();
	//cout << Char27out(serate.activeunits)<< "try 34 " << endl;
	int iret = 0;
	for (int iu = 0; iu < 27; iu++) if (serate.activeunits & (1 << iu))
		iret += DoEr34U(iu);
	if (iret) {
		serate.SetRating(34);
		//ImageCandidatsShort();
	}

	return iret;
}
int SOLV81::DoEr36U(int iu) {// naked triplet in unit
	uint32_t cleandone = 0;
	uint32_t bitiu = 1 << iu;
	BF128 uc = unsolved_cells & units3xBM[iu];
	BF128 uc2 = uc & (solve.sets.c2345[0]| solve.sets.c2345[1]),
		x=uc2;
	char ws[82]; ws[81] = 0;
	//need 3 out of  all cells  unsolved 2/3 digits 
	while (x.Count96() >= 3) {//get first cell
		int c1 = x.getFirstCell(), c1v = cells[c1];
		x.Clear_c(c1);
		BF128 y = x;
		while (y.Count96() >= 2) {// get second cell maxi 3 digits
			int c2 = y.getFirstCell(), c2v = cells[c2],
				v12=c1v|c2v,c3;
			y.Clear_c(c2);
			if (_popcnt32(v12) > 3) continue;
			BF128 z=y;
			while ((c3=z.getFirstCell())>=0) {// get third
				int v123 = v12 | cells[c3];
				z.Clear_c(c3);
				if (_popcnt32(v123) > 3) continue;
				// we have a naked triplet can have clean
				BF128 trip; trip.SetAll_0();
				trip.Set_c(c1); trip.Set_c(c2); trip.Set_c(c3);
				for (int i = 0; i < 9; i++) {
					if (!(v123 & (1 << i))) continue;
					BF128 dmi = (dm[i] & units3xBM[iu]) - trip;
					if (dmi.isNotEmpty()) {
						cleandone = 1;	Clean(i, dmi);
					}
				}
			}
		}
	}
	if (cleandone) cout << iu + 1 << " unit active NP3 " << endl;
	return  cleandone;
}
int SOLV81::DoEr36() {// Naked_triplet
	int iret = 0;
	for (int iu = 0; iu < 27; iu++) if (serate.activeunits & (1 << iu))
		iret += DoEr36U(iu);
	if (iret) 		serate.SetRating(36);
	return iret;
}
int SOLV81::DoEr38D(int d, int diag) {// swordfish
	int tri[9], tr[9], ntr = 0, iret = 0;
	BF128 w;
	if (diag) w = dmd[d] & unsdiag;
	else w = dm[d] & unsolved_cells;
	for (int i = 0; i < 9; i++){
		register uint32_t n = (w & units3xBM[i]).Count96();
		if (n && n <= 3) {
			int ib = i / 3, ir = i - 3 * ib;
			tri[ntr] = i;
			tr[ntr++] = (w.bf.u32[ib] >> 9 * ir) & 0x1ff;// row
		}
	}
	if (ntr < 3) return 0;
	for (int i = 0; i < ntr - 2; i++) {
		register int a = tr[i];
		for (int j = i + 1; j < ntr-1; j++) {
			register int ab = tr[j]|a;
			if (_popcnt32(ab) != 3) continue;// not swordfish
			for (int k = j + 1; k < ntr; k++) {
				tr[k];
				if (!((tr[k]| ab)==ab))continue;//must be 3 columns at the end
				int c1, c2, c3,x=ab;
				//cout << "gosw" << endl;
				bitscanforward(c1, ab);
				bitscanreverse(c2, ab);
				x ^= 1 << c1; bitscanforward(c3, x);
				BF128 wr = units3xBM[tri[i]]; wr |= units3xBM[tri[j]];
				BF128 wc = units3xBM[c1 + 9]; wc |= units3xBM[c2 + 9];
				wr |= units3xBM[tri[k]]; wc |= units3xBM[c3 + 9];
				wr &= w; wc &= w;	if (wr != wc) {
					wc -= wr;
					if (diag)CleanDiag(d, wc);
					else Clean(d, wc);
					iret++;
				}
			}
		}
	}
	return iret;
}
int SOLV81::DoEr38() {// swordfish
	int iret = 0;
	for (int id = 0; id < 9; id++) {// 9 digits
		if (!(serate.activedigits & (1 << id))) continue;
		iret += DoEr38D(id, 0) | DoEr38D(id, 1);
	}
	if (iret) serate.SetRating(38);
	return iret;
}
int SOLV81::DoEr40U(int iu) {// hidden triplet
	int tui[9], ntui = 0, iret = 0, cleandone = 0;
	BF128 wu = unsolved_cells & units3xBM[iu];
	for (int i = 0; i < 9; i++) { // 9 digits count <=3
		register uint32_t n = (wu & dm[i]).Count96();
		if (n && n <= 3) tui[ntui++] = i;
	}
	// all digits of unit unsolved 2/3 cells 
	for (int i = 0; i < ntui - 2; i++) {
		int d1 = tui[i];
		BF128 mu1 = wu & dm[d1];
		for (int j=i+1; j < ntui - 1; j++) {
			int d2 = tui[j];
			BF128 mu12 =mu1 |( wu & dm[d2]);
			if (mu12.Count96() > 3) continue;
			for (int k = j + 1; k < ntui; k++) {
				int d3 = tui[k];
				BF128 mu3 = wu & dm[d3];
				if ((mu3 - mu12).isNotEmpty()) continue;
				// HP3 see if active
				for (int i = 0; i < 9; i++) {
					if ((i == d1) || (i == d2) || (i == d3)) continue;
					BF128 z = (wu & dm[i]) & mu12;
					if (z.isNotEmpty()) {
						cleandone = 1;	Clean(i, z);
					}
				}
			}
		}
	}
	if (cleandone) {
		//cout << iu + 1 << " unit active HP3 " << endl;
		//ImageCandidatsShort();
	}

	return  cleandone;
}
int SOLV81::DoEr40() {// hidden_triplet
	//cout << " entry 40 HP" << endl;
	int iret = 0;
	for (int iu = 0; iu < 27; iu++) if (serate.activeunits & (1 << iu))
		iret += DoEr40U(iu);
	if (iret) 		serate.SetRating(40);
	return iret;
}
int SOLV81::DoEr42() {// XY Wing  
	int iret = 0 , c1, c2, c3, c1v, c2v, c3v, vd3, d1, d2, d3;
	BF128 w2 = solve.sets.c2345[0], x = w2;
	while ((c1 = x.getFirstCell()) >= 0) {// try ech cell as pivot
		x.Clear_c(c1);
		c1v = cells[c1];
		if (_popcnt32(c1v) != 2)continue;// killed earlier
		bitscanforward(d1, c1v);
		bitscanreverse(d2, c1v);
		BF128 seen = w2 & cell_z3x[c1],
			sd1 = seen & dm[d1], sd2 = seen & dm[d2], sc = sd1 & sd2;
		sd1 -= sc; sd2 -= sc;// no naked pair
		if (sd1.isEmpty() || sd2.isEmpty()) continue;
		char ws[82]; ws[81] = 0;
		while ((c2 = sd1.getFirstCell()) >= 0) {
			sd1.Clear_c(c2);
			c2v = cells[c2]; vd3 = c2v & ~c1v;
			bitscanforward(d3, vd3);
			BF128 y = sd2;
			while ((c3 = y.getFirstCell()) >= 0) {
				y.Clear_c(c3);
				c3v = cells[c3];
				if (!(c3v & vd3)) continue; // must be same extra digit
				BF128 zc = dm[d3] & unsolved_cells;
				zc &= cell_z3x[c2]; zc &= cell_z3x[c3];
				if (zc.isNotEmpty()) {
					iret = 1;
					//cout << zc.String3X(ws) << " clean " << d3 + 1 << endl;
					Clean(d3, zc);
				}
			}
		}
	}
	if (iret)serate.SetRating(42);
	return iret;
}
// must be in band or stack
int SOLV81::DoEr44BS(int ibs) {// B/S XYZ Wing
	int  c1, c2, c3, c1v, c2v,c3v, d1, d2, d3;
	BF128 w2 = solve.sets.c2345[0] & band3xBM[ibs];
	BF128 w3 = solve.sets.c2345[1] & band3xBM[ibs];
	while ((c1 = w3.getFirstCell()) >= 0) {// try each cell as pivot
		w3.Clear_c(c1);
		c1v = c2v = cells[c1];
		//cout << Char9out(c1v) << " cell " << c1 + 1 << endl;
		BF128 ss = cell_z3x[c1];
		bitscanforward(d1, c1v); c2v ^= 1 << d1;
		bitscanforward(d2, c2v);
		bitscanreverse(d3, c2v);
		// now all cells biv of the 3 digits seen
		int box = tcellsrcb3[c1][2];
		BF128 xd12 = ((dm[d1] & dm[d2]) & w2) & ss,
			xd13 = ((dm[d1] & dm[d3]) & w2) & ss,
			xd23 = ((dm[d2] & dm[d3]) & w2) & ss,
			xall = (xd12 | xd13) | xd23,
			xbox=xall&units3xBM[box];
		if (xbox.Count96() != 1)continue;
		if (xall.Count96() < 2)continue;
		c3 = xbox.getFirstCell();
		c2 = (xall - xbox).getFirstCell();
		c2v = cells[c2]; c3v = cells[c3];
		if (c2v == c3v) continue;
		int bitd = c2v & c3v,dx;
		bitscanforward(dx, bitd);
		BF128 zc=dm[dx]& units3xBM[box];
		zc &= cell_z3x[c2]; zc.Clear_c(c1); zc.Clear_c(c3);
		if (zc.isNotEmpty()) {
			Clean(dx, zc);
			return 1; //  only one per band stack
		}
	}
	return 0;
}
int SOLV81::DoEr44() {// XYZ Wing only one per cycle
	for (int ibs = 0; ibs < 6; ibs++){
		if (DoEr44BS(ibs)) {

			serate.SetRating(44);		return 1;		}
	}
	return 0;
}
// 45... in ur_ul
int SOLV81::DoEr50U(int iu) {// naked quad in unit
	//cout << "n4 iu=" << iu + 1 << endl;
	uint32_t cleandone = 0;
	//uint32_t bitiu = 1 << iu;
	BF128 uc = unsolved_cells & units3xBM[iu];
	BF128 uc2 = uc & (solve.sets.c2345[0] | solve.sets.c2345[1]
		| solve.sets.c2345[2]),		x = uc2;
	char ws[82]; ws[81] = 0;
	//cout << x.String3X(ws) << "uc2 " << endl;
	//need 3 out of  all cells  unsolved 2/3 digits 
	while (x.Count96() >= 4) {//get first cell
		int c1 = x.getFirstCell(), c1v = cells[c1];
		x.Clear_c(c1);
		BF128 y2 = x;
		while (y2.Count96() >= 3) {// get second cell maxi 3 digits
			int c2 = y2.getFirstCell(), c2v = cells[c2],
				v12 = c1v | c2v;
			y2.Clear_c(c2);
			if (_popcnt32(v12) > 4) continue;
			BF128 y3 = y2;
			while (y3.Count96() >= 2) {// get third
				int c3 = y3.getFirstCell(),
				    v123 = v12 | cells[c3],c4;
				y3.Clear_c(c3);
				if (_popcnt32(v123) > 4) continue;
				BF128 y4 = y3;
				while ((c4 = y4.getFirstCell()) >= 0) {// get third
					int v1234 = v123 | cells[c4];
					y4.Clear_c(c4);
					if (_popcnt32(v1234) > 4) continue;
					// we have a naked quad can have clean
					BF128 quad; quad.SetAll_0();
					quad.Set_c(c1); quad.Set_c(c2); 
					quad.Set_c(c3); quad.Set_c(c4);
					for (int i = 0; i < 9; i++) {
						if (!(v1234 & (1 << i))) continue;
						BF128 dmi = (dm[i] & units3xBM[iu]) - quad;
						if (dmi.isNotEmpty()) {
							cleandone = 1;	Clean(i, dmi);
						}
					}
				}
			}
		}
	}
	if (cleandone) cout << iu + 1 << " unit active NP4 " << endl;
	return  cleandone;
}
int SOLV81::DoEr50() {// Naked_quad
	//cout << "doer50 n4" << endl;
	int iret = 0;
	for (int iu = 0; iu < 27; iu++) if (serate.activeunits & (1 << iu))
		iret += DoEr50U(iu);
	if (iret) 		serate.SetRating(50);
	return iret;
}
int SOLV81::DoEr52D(int d, int diag) {// jellyfish
	//char ws[82]; ws[81] = 0;
	int tri[9], tr[9], ntr = 0, iret = 0;
	BF128 w;
	if (diag) w = dmd[d] & unsdiag;
	else w = dm[d] & unsolved_cells;
	//cout << w.String3X(ws) << " DoEr52D " << d + 1 << " " << diag << endl;
	for (int i = 0; i < 9; i++) {
		register uint32_t n = (w & units3xBM[i]).Count96();
		if (n && n <= 4) {
			//cout << i << "ok n "<<n << endl;
			int ib = i / 3, ir = i - 3 * ib;
			tri[ntr] = i;
			tr[ntr++] = (w.bf.u32[ib] >> 9 * ir) & 0x1ff;// row
		}
	}
	if (ntr < 4) return 0;
	//cout << " jelly ntr=" << ntr << endl;
	for (int i = 0; i < ntr - 3; i++) {
		register int a = tr[i];
		for (int j = i + 1; j < ntr - 2; j++) {
			register int ab = tr[j] | a;
			if (_popcnt32(ab)  > 4) continue;// not jellyf
			for (int k = j + 1; k < ntr-1; k++) {
				register int abc = tr[k] | ab;
				if (_popcnt32(abc) > 4) continue;// not jellyfish				
				for (int l = k + 1; l < ntr; l++) {
					register int a4 = tr[l] | abc;
					if (_popcnt32(a4) != 4)continue;//must be 4 columns at the end
					int c1, c2, c3,c4, x = a4;
					//cout <<Char9out(a4)<< "gojelly d "<<d+1   << " diag " <<diag << endl;
					bitscanforward(c1, x);
					bitscanreverse(c2, x);
					x ^= 1 << c1; x ^= 1 << c2;
					bitscanforward(c3, x);
					bitscanreverse(c4, x);
					BF128 wr = units3xBM[tri[i]]; wr |= units3xBM[tri[j]];
					BF128 wc = units3xBM[c1 + 9]; wc |= units3xBM[c2 + 9];
					wr |= units3xBM[tri[k]]; wc |= units3xBM[c3 + 9];
					wr |= units3xBM[tri[l]]; wc |= units3xBM[c4 + 9];
					wr &= w; wc &= w;	
					if (wr != wc) {
						wc -= wr;
						if (diag)CleanDiag(d, wc);
						else Clean(d, wc);
						iret++;
					}
				}
			}
		}
	}
	return iret;
}
int SOLV81::DoEr52() {// jellyfish
	int iret = 0;
	for (int id = 0; id < 9; id++) {// 9 digits
		if (!(serate.activedigits & (1 << id))) continue;
		iret += DoEr52D(id, 0) | DoEr52D(id, 1);
	}
	if (iret) serate.SetRating(52);
	return iret;
}
int SOLV81::DoEr54U(int iu) {// hidden quad
	int tui[9], ntui = 0, iret = 0, cleandone = 0;
	BF128 wu = unsolved_cells & units3xBM[iu];
	for (int i = 0; i < 9; i++) { // 9 digits count <=3
		register uint32_t n = (wu & dm[i]).Count96();
		if (n && n <= 4) tui[ntui++] = i;
	}
	// all digits of unit unsolved 2/4 cells 
	//char ws[82]; ws[81] = 0;
	for (int i = 0; i < ntui - 3; i++) {
		int d1 = tui[i];
		BF128 mu1 = wu & dm[d1];
		//cout << mu1.String3X(ws) << " mu1 " << endl;
		for (int j = i + 1; j < ntui - 2; j++) {
			int d2 = tui[j];
			BF128 mu12 = mu1 | (wu & dm[d2]);
			if (mu12.Count96() > 4) continue;
			//cout << mu12.String3X(ws) << " mu12 " << endl;
			for (int k = j + 1; k < ntui-1; k++) {
				int d3 = tui[k];
				BF128 mu123 = mu12 | (wu & dm[d3]);
				if (mu123.Count96() > 4) continue;
				for (int l = k + 1; l < ntui; l++) {
					int d4 = tui[l];
					BF128 m4 = mu123 | (wu & dm[d4]);
					if (m4.Count96() != 4) continue;
					//cout<< iu+1 << " HP4 see if active" << endl;
					//cout << m4.String3X(ws) << " m4 " << endl;
					for (int i = 0; i < 9; i++) {
						if ((i == d1) || (i == d2) 
							|| (i == d3) || (i == d4)) continue;
						BF128 z = (wu & dm[i]) & m4;
						//cout << z.String3X(ws) << " z d= "<<i+1 << endl;
						if (z.isNotEmpty()) {
							cleandone = 1;	Clean(i, z);
						}
					}
				}
			}
		}
	}
	if (cleandone) {
		//cout << iu + 1 << " unit active HP4 " << endl;
		//ImageCandidatsShort();
	}

	return  cleandone;
}
int SOLV81::DoEr54() {// hidden_triplet
	//cout << " entry 54 HP4" << endl;
	int iret = 0;
	for (int iu = 0; iu < 27; iu++) if (serate.activeunits & (1 << iu))
		iret += DoEr54U(iu);
	if (iret) 		serate.SetRating(54);
	return iret;
}

// bugs see bugs file
// pair triplet exclusion see the file
// xchains see xchain file
// ychains see ychain file
int SOLV81::DoEr70() {
	return xybiv.Init();
}
int SOLV81::DoEr7x(int rat) {
	return xybiv.Er71();
} 
