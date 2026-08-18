#include "skfr2c_solveSerate.h"
#include "skfr2c_solv81_cpp.h"

//===================================                solve functions

int SOLVE::Init(char* ze) {
	// check entry, store solution where needed
	{
		memcpy(puz, ze, 81); puz[81] = 0;
		if (!SkbfCheckValidityQuick(puz)) 	return 1;
		for (int i = 0; i < 81; i++) skfr2ii.sol[i]= pzhe->gsol[i] + '1';
		memcpy(g0, pzhe->gsol, sizeof g0);
		sv81w.Init(g0);
	}
	return sv81w.GoSudoku(puz);
}
void SOLVE::Clean(int dig, BF128& c) { sv81w.Clean(dig, c); }
void SOLVE::CleanCell(int cell,int digs) { sv81w.CleanCell(cell,digs); }

int SOLVE::SinglesAtStart() {
	int a = sv81w.unsolved_cells.Count96();
	sv81w.DoSingles();
	return  (a == sv81w.unsolved_cells.Count96());
}

int  SOLVE::SolveMinis() {
	minis.InitSolPerDigit(g0);
	//minis.DumpSolsPerDigit1();
	if (minis.Build(*this)) return 1;
	//minis.Dump1();
	return 0;
}
void SOLVE::SetsBuild() {
	if (solve.opp & 4)cout << "SetsBuild() " << endl;
	sets.Build(*this);
}
int SOLVE::SolveUnits2c() {
	uint32_t cleandone = 0;
	for (int iu = 0; iu < 27; iu++) {
		uint32_t bitiu = 1 << iu;
		BF128 uc = sv81w.unsolved_cells & units3xBM[iu];
		BF128 uc2 = uc & solve.sets.c2345[0];
		if (uc2.Count96() < 2) continue;
		while (1) {
			int c1 = uc2.getFirstCell(), c1v = solve.sv81w.cells[c1];
			uc2.Clear_c(c1);
			BF128 uc2b = uc2;
			int tc[10], ntc = uc2b.Table3X27(tc);
			for (int ic2 = 0; ic2 < ntc; ic2++) {
				int c2 = tc[ic2], c2v = solve.sv81w.cells[c2] ;
				if (c2v != c1v) continue;
				uc2.Clear_c(c2);// forget it now
				int d1, d2;
				bitscanforward(d1, c1v);
				bitscanreverse(d2, c1v);
				DSETS& ds1 = solve.sets.ds[d1], & ds2 = solve.sets.ds[d2];
				uint32_t uds1 = ds1.d234m.bf.u32[0],
					uds2 = ds2.d234m.bf.u32[0],udsc=uds1&uds2;
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
	}
	if (cleandone) {
		return 1;
	}
	return  0;
}
int SOLVE::SolveUnits2h() {
	uint32_t cleandone = 0;
	for (int iu = 0; iu < 27; iu++) {
		uint32_t bitiu = 1 << iu;
		BF128 uc = sv81w.unsolved_cells & units3xBM[iu];

		for (int ifl = 0; ifl < 36; ifl++) {
			int fl = floors_2d[ifl],d1,d2;
			bitscanforward(d1, fl);		bitscanreverse(d2, fl);
			DSETS& ds1 = solve.sets.ds[d1], & ds2 = solve.sets.ds[d2];
			uint32_t uds1 = ds1.d234m.bf.u32[0],
				uds2 = ds2.d234m.bf.u32[0], udsc = uds1 & uds2;
			if (!(udsc & bitiu) )	continue;// must be both pair in unit
			BF128 sd1 = ds1.rcb[iu], sd2 = ds2.rcb[iu];
			if (sd1 != sd2) continue;// not same cells
			int c1 = sd1.getFirstCell(), c2 = sd1.getLastCell();
			int v1 = solve.sv81w.cells[c1]& ~fl ,
				v2 = solve.sv81w.cells[c2] & ~fl;
			if (v1) {
				cleandone = 1;
				CleanCell(c1,v1);
			}
			if (v2) {
				cleandone = 1;
				CleanCell(c2, v2);
			}
		}
	}
	if (cleandone) 	return 1;	
	return  0;
}
int SOLVE::SolveUnits3c() {
	uint32_t cleandone = 0;
	for (int iu = 0; iu < 27; iu++) {
		cout << "try iu =" << iu + 1 << endl;
		uint32_t bitiu = 1 << iu;
		BF128 uc = sv81w.unsolved_cells & units3xBM[iu];
		BF128 uc2 = uc & solve.sets.c2345[0];
		if (uc2.Count96() < 2) continue;
		while (1) {
			int c1 = uc2.getFirstCell(), c1v = solve.sv81w.cells[c1];
			cout << " try cell " << c1 + 1 << endl;
			uc2.Clear_c(c1);
			BF128 uc2b = uc2;
			int tc[10], ntc = uc2b.Table3X27(tc);
			for (int ic2 = 0; ic2 < ntc; ic2++) {
				int c2 = tc[ic2], c2v = solve.sv81w.cells[c2];
				if (c2v != c1v) continue;
				uc2.Clear_c(c2);// forget it now
				int d1, d2;
				bitscanforward(d1, c1v);
				bitscanreverse(d2, c1v);
				cout << iu + 1 << " " << d1 + 1 << " " << d2 + 1 << " iu digits to see cells" << endl;
				DSETS& ds1 = solve.sets.ds[d1], & ds2 = solve.sets.ds[d2];
				uint32_t uds1 = ds1.d234m.bf.u32[0],
					uds2 = ds2.d234m.bf.u32[0], udsc = uds1 & uds2;
				if (udsc & bitiu) 	continue;// no clearing
				BF128 cx; cx.SetAll_0(); cx.Set_c(c1); cx.Set_c(c2);
				BF128 clx = ds1.rcb[iu] - cx;
				if (clx.isNotEmpty()) {
					cout << "clean d1" << endl;
					cleandone = 1;
					Clean(d1, clx);
				}
				clx = ds2.rcb[iu] - cx;
				if (clx.isNotEmpty()) {
					cout << "clean d2" << endl;
					cleandone = 1;
					Clean(d2, clx);
				}
			}
			if (uc2.Count96() < 2) break;
		}
	}
	if (cleandone) {
		cout << "cleandone on cells loop" << endl;
		return 1;
	}
	return  0;
}
int SOLVE::SolveUnits3h() {
	uint32_t cleandone = 0;
	for (int iu = 0; iu < 27; iu++) {
		cout << "try iu =" << iu + 1 << endl;
		uint32_t bitiu = 1 << iu;
		BF128 uc = sv81w.unsolved_cells & units3xBM[iu];

		for (int ifl = 0; ifl < 84; ifl++) {
			int fl = floors_3d[ifl];
			const char* myv = t512v[fl];
			int d1 = myv[0] - '1', d2 = myv[1] - '1', d3 = myv[2] - '1';
			DSETS& ds1 = solve.sets.ds[d1], & ds2 = solve.sets.ds[d2],
				ds3 = solve.sets.ds[d3];
			uint32_t uds1 = ds1.d234m.bf.u32[0],
				uds2 = ds2.d234m.bf.u32[0], udsc = uds1 & uds2;
			if (!(udsc & bitiu))	continue;// must be both pair in unit
			BF128 sd1 = ds1.rcb[iu], sd2 = ds2.rcb[iu];
			if (sd1 != sd2) continue;// not same cells
			cout << iu + 1 << " " << d1 + 1 << " " << d2 + 1 << " go clean" << endl;

			int c1 = sd1.getFirstCell(), c2 = sd1.getLastCell();
			int v1 = solve.sv81w.cells[c1] & ~fl,
				v2 = solve.sv81w.cells[c2] & ~fl;
			if (v1) {
				cout << "clean c1 " << c1 << endl;
				cleandone = 1;
				CleanCell(c1, v1);
			}
			if (v2) {
				cout << "clean c2 " << c2 << endl;
				cleandone = 1;
				CleanCell(c2, v2);
			}
		}
	}
	if (cleandone) {
		cout << "cleandone on hidden pairs loop" << endl;
		return 1;
	}
	return  0;
}
#ifdef SEROUT
#endif
//______________ solve any to clear in one floor
int SOLVE::SolveF1(int opt,int modeserate) {// Solve one floor in SLG
	int iret = 0;
	SLG slg; slg.InitFromSolve();
	for (int idig = 0; idig < 9; idig++) {
		DSETS& ds = sets.ds[idig];
		slg.lfield[0] = sv81w.dm[idig] & sv81w.unsolved_cells;
		slg.ntsd = 0;
		slg.dig1 = idig;
		for (int ic = 0; ic < 4; ic++) {// sets rows by size 
			uint32_t ux = ds.d234m.bf.u32[ic];
			if (!ux) continue;
			for (int iu = 0; iu < 9; iu++) if (ux & (1 << iu)) {
				slg.tsd[slg.ntsd++] = ds.rcb[iu];
			}
		}
		if (slg.ntsd < 2) continue; // at least 2 rows unassigned 
		int more= slg.Expand1_sd_ld(opt,modeserate);// row col if 1
		if (more == 2) {
#ifdef SEROUT
			cout << " no valid fill for digit " << idig + 1 << endl;
			slg.sv.ImageOne(idig, 1);
#endif
			sv81w.bug = 1;
			return iret;
		}
		if (more) iret |= 1 << idig;
#ifdef SEROUT
		cout << "active  floor digit " << idig + 1 << endl;
		sv81w.ImageOne(idig, 1);
#endif
	}
	memcpy(rclean1, slg.rclean1, 9 * sizeof rclean1[0]);
	return iret;
}
//______________ solve any to clear in one unit
int SOLVE::SolveU1(int modeserate) {// Solve all one unit
	int iret = 0;
	for (int iu = 0; iu < 27; iu++) {// 27 unit
		//cout << "u1 iu " <<iu+1<< endl;
		SLG slg; slg.InitFromSolve();
		BF128 cfield = sv81w.unsolved_cells& units3xBM[iu];
		// store sets by size
		int all_digs = 0;
		for (int ic = 0; ic < 4; ic++) {// sets by size 
			BF128 x= cfield &sets.c2345[ic];
			int cell;
			while ((cell = x.getFirstCell()) >= 0) {
				x.Clear_c(cell);
				slg.tsc[slg.ntsc++]=cell;
				all_digs |= sv81w.cells[cell];
			}
		}
		if (slg.ntsc < 3) continue;
		// add digit links for this unit 
		BF128* wl = slg.lfield;
		memset(wl, 0, sizeof slg.lfield);
		for (int idig = 0; idig < 9; idig++) {
			if (!(all_digs & (1 << idig)))continue;
			wl[idig] |= (cfield & sv81w.dm[idig]);
		}
		// now expand/clean 
		int more = slg.Expand_sc_ld(modeserate);		
		if(more)  iret |= 1<<iu;
	}
	return iret;
}
int SOLVE::SolveU1SC() {// Solve all one unit
	char ws[82]; ws[81] = 0;
	if (opp & 4) cout << "entry solve u1sc" << endl;
	int locdiag = 0;
	int iret = 0;
	for (int iu = 0; iu < 27; iu++) {// 27 unit
		//if (iu == 11)locdiag = 1; else locdiag = 0;
		if(locdiag)cout << "u1 iu " << iu + 1 << endl;
		SLG slg; slg.InitFromSolve();
		BF128 cfield = sv81w.unsolved_cells & units3xBM[iu];
		if (locdiag)cout <<cfield.String3X(ws) <<" unsunit" << endl;		// store sets by size
		int all_digs = 0;
		for (int ic = 0; ic < 4; ic++) {// sets by size 
			BF128 x = cfield & sets.c2345[ic];
			int cell;
			while ((cell = x.getFirstCell()) >= 0) {
				x.Clear_c(cell);
				slg.tsc[slg.ntsc++] = cell;
				all_digs |= sv81w.cells[cell];
			}
		}
		if(locdiag)cout << Char9out(all_digs)
			<<" end sets  " << slg.ntsc << endl;
		if (slg.ntsc < 3) continue;
		// add digit links for this unit 
		BF128* wl = slg.lfield;
		memset(wl, 0, sizeof slg.lfield);
		for (int idig = 0; idig < 9; idig++) {
			if (!(all_digs & (1 << idig)))continue;
			BF128 w = cfield & sv81w.dm[idig];
			wl[idig] |= w;
			if (locdiag)cout <<  w.String3X(ws) << " i="<<idig+1 << endl;
		}
		// now expand/clean 
		int more = slg.Expand_sc_ld(locdiag);
		iret += more;
		if (opp & 4 && more) cout << "active  unit " << iu + 1 << endl;
		
	}
	return iret;
}
//______________ solve any to clear with cells bi values
int SOLVE::IsYbiv(int serate ) {// Solve all cells biv as sets
	//if (opp & 8) 
	//cout << "entry solve ybiv" << endl;
	slgybiv.InitFromSolve();
	memset(slgybiv.lfield, 0, sizeof slgybiv.lfield);
	memset(slgybiv.orf, 0, sizeof slgybiv.orf);
	BF128 dmu[9];
	for (int i = 0; i < 9; i++)dmu[i] = sv81w.dm[i] & sv81w.unsolved_cells;
	// get cellls bi values as sets and link field
	{
		BF128 x = sets.c2345[0];// cells bi values 
		if (x.Count96() < 3) return 0;
		int cell;
		while ((cell = x.getFirstCell()) >= 0) {
			x.Clear_c(cell);
			slgybiv.tsc[slgybiv.ntsc++] = cell;
			// take as link field all seen candidates
			int digs = sv81w.cells[cell],d1,d2;
			bitscanforward(d1, digs);
			bitscanreverse(d2, digs);
			{
				BF128& w = slgybiv.lfield[d1];
				w.Set_c(cell);	w |= dmu[d1] &cell_z3x[cell];
			}
			{
				BF128& w = slgybiv.lfield[d2];
				w.Set_c(cell);	w |=dmu[d2] & cell_z3x[cell];
			}
		}
	}
	//slgybiv.Status(1);
	int ir= slgybiv.Expand_sc_ld(serate); 
	//if (ir&&serate)slgybiv.DumpElims();
	return  ir;
}

//______________ solve   all xy chains/loops
int SOLVE::IsAllBiv(int serate) {// Solve all xy chains
	if (opp & 8) cout << "entry solve all biv" << endl;
	SLG slg; slg.InitFromSolve();
	memset(slg.lfield, 0, sizeof slg.lfield);
	memset(slg.orf, 0, sizeof slg.orf);
	// get cellls bi values as sets and link field
	{
		BF128 x = sets.c2345[0];// cells bi values 
		//if (x.Count96() < 3) return 0;
		int cell;
		while ((cell = x.getFirstCell()) >= 0) {
			x.Clear_c(cell);
			slg.tsc[slg.ntsc++] = cell;
			// take as link field all seen candidates
			int digs = sv81w.cells[cell], d1, d2;
			bitscanforward(d1, digs);
			bitscanreverse(d2, digs);
			{
				BF128& w = slg.lfield[d1];
				w.Set_c(cell);	w |= cell_z3x[cell];
			}
			{
				BF128& w = slg.lfield[d2];
				w.Set_c(cell);	w |= cell_z3x[cell];
			}
		}
	}
	// get digit biv and cells to use as links
	BF128  dig_cells_hit[9];
	memset(dig_cells_hit, 0, sizeof dig_cells_hit);
	//return 0;
	// get digits bi values as sets and link field
	for (int idig = 0; idig < 9; idig++) {
		DSETS& ds = solve.sets.ds[idig];
		uint32_t ux = ds.d234m.bf.u32[0];// digit biv units
		while (ux) {
			uint32_t u;
			bitscanforward(u, ux);
			ux ^= 1 << u;// clear bit
			BF128 w = ds.rcb[u], wr = w;
			wr.bf.u32[3] = idig;
			int cell1 = w.getFirstCell();
			w.Clear_c(cell1);
			int cell2 = w.getFirstCell();
			if (u >= 18) {//ignore box redundant
				if (cell1 / 9 == cell2 / 9) continue;// same row
				if (cell1 % 9 == cell2 % 9) continue;// same col
			}
			dig_cells_hit[idig].Set_c(cell1);
			dig_cells_hit[idig].Set_c(cell2);
			slg.tsd[slg.ntsd++] = wr;//row col or  pure box bi value
		}
	}
	// add as link all cells hit twice (different digits 
	slg.InitLinks();
	BF128 R1 = dig_cells_hit[0], R2; R2.SetAll_0();
	for (int i = 1; i < 9; i++) {
		BF128 x = dig_cells_hit[i];
		R2 |= x & R1; R1 |= x;
	}
	{
		int cell;
		while ((cell = R2.getFirstCell()) >= 0) {
			R2.Clear_c(cell);
			slg.AddClink(cell);
		}
	}

	return slg.Expand( serate);
}
// ________________int SolveUR()___ ur ul via 2  cells biv as base 
struct WUR {
	SOLV81 svr, svf;
	BF128 x,x2d,x2dbs,my2d,my2dbs;
	int digs,d1, d2,cell, c1, c2, c3, c4;
	struct ED {
		int dig; BF128 c;
	}ted[20];	
	void CleanCell(int cell, int digs) {
		svf.cells[cell] &= ~digs;
		svf.dm[d1].Clear_c(cell);
		svf.dm[d2].Clear_c(cell);
	}	

}wur;
int SOLVE::SolveUR() {// select first all 2 digits cells bivalues
	wur.svr= sv81w; wur.svf = sv81w;
	wur.x = sets.c2345[0];// cells bi values
	while (wur.x.Count96() > 1) {
		wur.cell = wur.x.getFirstCell();
		wur.digs = wur.svr.cells[wur.cell];
		bitscanforward(wur.d1, wur.digs);
		bitscanreverse(wur.d2, wur.digs);
		wur.x2d = (wur.svr.dm[wur.d1] & wur.svr.dm[wur.d2])
			& wur.svr.unsolved_cells;
		wur.my2d = wur.x2d & wur.x;
		wur.x.Clear_c(wur.cell);
		if (wur.my2d.Count96() < 2) continue;
		SolveUR_b();
	}
	int aig = 0;
	for(int i=0;i<9;i++)if(wur.svr.dm[i]!= wur.svf.dm[i])
	{	aig = 1; break;	}
	if (aig) {	sv81w = wur.svf; return 1;	}	
	return 0; }
int SOLVE::SolveUL() {// select first all 2 digits cells bivalues
	//cout << "entry solveUL " << endl;
	wur.svr = sv81w; wur.svf = sv81w;
	wur.x = sets.c2345[0];// cells bi values
	while (wur.x.Count96() > 4) { // min 6 cells max 2 with extra digits
		wur.cell = wur.x.getFirstCell();
		wur.digs = wur.svr.cells[wur.cell];
		bitscanforward(wur.d1, wur.digs);
		bitscanreverse(wur.d2, wur.digs);
		//cout << " try cell " << wur.cell + 1 << " " << wur.d1 + 1 << " " << wur.d2 + 1 << endl;
		wur.x2d = (wur.svr.dm[wur.d1] & wur.svr.dm[wur.d2])
			& wur.svr.unsolved_cells;
		wur.my2d = wur.x2d & wur.x;// all bi values for this pair
		if (wur.my2d.Count96() < 5) {// no UL here kill and loop
			wur.x -= wur.my2d;		continue;	}
		// must have one row on one col 2 digits in box as start
		BF128 wu;
		int aig = 0,cell1,cell2,iunit;
		for (iunit = 0; iunit < 18; iunit++) {
			wu = units3xBM[iunit];		wu &= wur.my2d;
			if (wu.Count96() != 2) continue;
			cell1 = wu.getFirstCell(); cell2 = wu.getLastCell();
			//must be same box 
			if (C_box[cell1] != C_box[cell2]) continue;
			else { wur.x -=wu; aig = 1; break;		}
		}
		if (!aig) {// no start, kill and loop
			wur.x -= wur.my2d;		continue;	}
		 {// we have a start expand in rows 2 ends
			int usedunits = 1 << iunit, rc = iunit / 9,nbox[9];
			memset(nbox, 0, sizeof nbox); // check parity cells hits
			// wu is the start of used bi values expand now both ends
			int mycell = cell1, myrc = rc,nextunit;
			while (1) {//expand first
				nextunit = (myrc) ? mycell/ 9 : (mycell % 9) + 9; 
				int bit = 1 << nextunit;
				if (bit & usedunits) { cout << "loop" << endl; return 0; }// bug
				usedunits |= bit;
				BF128 wu2= units3xBM[nextunit];		wu2 &= wur.my2d;
				if (wu2.Count96() != 2) break;// end of branch	
				wur.x -=wu2;
				wu2.Clear_c(mycell);
				mycell = wu2.getFirstCell();
				nbox[C_box[mycell]]++;
				myrc = 1 - myrc;
			}
			//cout << "end branch 1 cell " << mycell + 1 << "  rc " << myrc << endl; 
			int mycell2 = cell2, myrc2 = rc, nextunit2;
			while (1) {//expand first
				nextunit2 = (myrc2) ? mycell2 / 9 : (mycell2 % 9) + 9;
				int bit = 1 << nextunit2;
				if (bit & usedunits) { cout << "loop" << endl; return 0; }// bug
				usedunits |= bit;
				BF128 wu2 = units3xBM[nextunit2];		wu2 &= wur.my2d;
				if (wu2.Count96() != 2)break; // end of branch				
				wur.x -= wu2;
				wu2.Clear_c(mycell2);
				mycell2 = wu2.getFirstCell();
				nbox[C_box[mycell2]]++;
				myrc2 = 1 - myrc2;
			}
			//cout << "end branch 2 cell " << mycell2 + 1 << "  rc " << myrc2 << endl;
			if (myrc != myrc2) {// should be one cell
				int cell4,pbox=1;
				if (myrc) 	cell4 = 9 * (mycell / 9) + mycell2 % 9;
				else cell4 = 9 * (mycell2 / 9) + mycell % 9;
				//cout << " ul 1 cell " << cell4 + 1 << endl;
				// must be boxes pair after cell4
				nbox[C_box[cell4]]++;
				for (int i = 0; i < 9; i++)if (nbox[i] & 1) { pbox = 0; break; }
				if (pbox) {
					//cout << "url active 1" << endl;
					wur.CleanCell(cell4, wur.digs);
				}
				continue;
			}
			//  now can be 2 cells in row or col
			if (myrc) {// next is row both end
				BF128 wxr = units3xBM[nextunit]; wxr |= units3xBM[nextunit2];
				wxr&= wur.x2d;
				// if all boxes pair, look in band 
				int nodd = 0;
				for (int i = 0; i < 9; i++)if (nbox[i] & 1) nodd++;
				if (nodd > 2 || nodd == 1)continue; // not the pattern
				if (nodd==2) {// must be 2 box in stack
					int ibx[2], n = 0;
					for (int i = 0; i < 9; i++)if (nbox[i] & 1) ibx[n++]=i;
					if (ibx[0]%3 == ibx[1]%3) {//must be to have one col
						BF128 wx = units3xBM[ibx[0]];
						wx |= units3xBM[ibx[1]];	wx &=wxr; 						
						if (wx.Count96() == 2) {// only one column can fit
							//cout << "this is a column 2 cells" << endl;
						}
					}
				}
				else { //must be same band one col free box
					int band1 = (mycell / 9) / 3, band2 = (mycell2 / 9) / 3;
					if (band1 == band2) {
						int stack1 = (mycell % 9) / 3, stack2 = (mycell2 % 9) / 3;
						for (int ist = 0; ist < 3; ist++) {
							if (ist == stack1 || ist == stack2) continue;
							int col0 = 3 * ist + 9;
							for (int icol = 0; icol < 3; icol++) {
								int col = col0 + icol;
								BF128 wrc=wxr& units3xBM[col];
								if (wrc.Count96() == 2) { 
									//cout << "this is a column 2 cells" << endl;
								}
							}
						}
					}
				}
			}
			else {// next is col both end 
				BF128 wxc = units3xBM[nextunit]; wxc |= units3xBM[nextunit2];
				wxc&= wur.x2d;
				// if all boxes pair, look in stack
				int nodd = 0;
				for (int i = 0; i < 9; i++)if (nbox[i] & 1) nodd++;
				if (nodd > 2 || nodd == 1)continue; // not the pattern
				if (nodd == 2) {// must be 2 box in band
					int ibx[2], n = 0;
					for (int i = 0; i < 9; i++)if (nbox[i] & 1) ibx[n++] = i;
					if (ibx[0] % 3 == ibx[1] % 3) {//must be to have one col
						BF128 wx = units3xBM[ibx[0]];
						wx |= units3xBM[ibx[1]];	wx &= wxc;
						if (wx.Count96() == 2) {// only one column can fit
							//cout << "this is a row 2 cells" << endl;
						}
					}
				}
				else { //must be same stack one row free box
					int stack1 = (mycell % 9) / 3, stack2 = (mycell2 % 9) / 3;
					if (stack1 == stack2) {
						int band1 = (mycell / 9) / 3, band2 = (mycell2 / 9) / 3;
						for (int ib = 0; ib < 3; ib++) {
							if (ib == band1 || ib == band2) continue;
							int row0 = 3 * ib;
							for (int irow = 0; irow < 3; irow++) {
								int row = row0 + irow;
								BF128 wcr = wxc & units3xBM[row];
								if (wcr.Count96() == 2) {
									//cout << "this is a row 2 cells" << endl;
								}
							}
						}
					}
				}

			}
		}
	}
	int aig = 0;
	for (int i = 0; i < 9; i++)if (wur.svr.dm[i] != wur.svf.dm[i])
	{
		aig = 1; break;
	}
	if (aig) { sv81w = wur.svf; return 1; }
	return 0;
}

void SOLVE::SolveUR_b() {//  base row or col
	//cout << "entry UR_b  cell "<< wur.cell+1 << endl;
	BF128 y = wur.my2d;// cell bi values 
	sv81w = wur.svr; // toujours partir de initial;
	int cell1 = wur.cell, row = cell1 / 9, col = cell1 % 9,
		band1 = row / 3, stack1 = col / 3;
	y.Clear_c(cell1);
	BF128 yr = y & units3xBM[row], yc = y & units3xBM[col + 9];
	BF128 yrc = yr | yc;
	if (yrc.isEmpty()) return;;// not a base 2 in row or 2 in col
	wur.x -= yrc;// clear all cells of this potential UR UL as start
	if (yrc.Count96() == 2) {//type 1 UR if  
		int cell2 = yr.getFirstCell(), cell3 = yc.getFirstCell();
		int row2 = cell3 / 9, col2 = cell2 % 9, cell4 = 9 * row2 + col2;
		if ((band1 != row2 / 3) && (stack1 != col2 % 3))return; // not UR 
		if (wur.x2d.Off_c(cell4)) return; // not a UR  
		//wur.AddCleanCell(cell4, wur.digs,45);
		//cout << "ur1 direct " << wur.cell + 1 << " " << cell4 + 1 << endl;
		return;
	}
	// now a start base 2 r/c
	if (yr.isNotEmpty()) {// row base search col 
		//cout << "row base search" << endl;
		int cell2 = yr.getFirstCell(), col2 = cell2 % 9,
			stack2 = col2 / 3, ra = 0, rb = 9;
		BF128 w = units3xBM[col + 9]; w |= units3xBM[col2 + 9];
		wur.x2dbs = wur.x2d & w;
		if (stack1 != stack2) {//search in band only
			ra = 3 * band1; rb = ra + 3;
		}
		for (int r = ra; r < rb; r++)if (r != row) {
			BF128 wr = wur.x2dbs & units3xBM[r], wrb = wr & wur.my2d;
			if (wr.Count96() == 2) {// one elem or UR1 to see
				int cell3 = wr.getFirstCell(), cell4 = wr.getLastCell();
				if (wrb.isNotEmpty()) {// UR1
					wur.x.Clear_c(cell4); SolveUR4(cell2, cell4, cell1, cell3);
					continue;
				}
				continue;
			}
		}
	}
	else { // column base search rows 
		int cell2 = yc.getFirstCell(),	row2 = cell2 / 9,
			band2=row2/3,ca=0,cb=9;
		//cout << "col base search cell2="<<cell2+1 << endl;
		BF128 w = units3xBM[row]; w |= units3xBM[row2];
		wur.x2dbs = wur.x2d & w;
		if (band1 != band2) {//search in band only
			ca = 3 * stack1; cb = ca + 3;
		}
		//cout << "search cols " << ca+1 << " " << cb+1 << endl;
		for (int c = ca; c < cb ; c++)if (c != col) {
			BF128 wc = wur.x2dbs & units3xBM[c + 9], wcb = wc & wur.my2d;
			if (wc.Count96() == 2) {// one elem to see
				int cell3 = wc.getFirstCell(), cell4 = wc.getLastCell();
				//cout << "ur seen cbase  " << wur.cell + 1 << endl;
				if (wcb.isNotEmpty()) {//UR1
					wur.x.Clear_c(cell4); SolveUR4(cell2, cell4, cell1, cell3);
					//cout << " turn to ur1" << endl;
					continue;
				}
				continue;
			}
		}
	}
}
void SOLVE::SolveUR4(int c1, int c2, int c3, int c4) {	// c1 c2 c3 c4 2/3 biv
	// c3 pair UR1
	// c3c4 el (2) dig biv or     hidden/naked xx
	if (sets.c2345[0].On_c(c3)) {//  bi values this is UR1
		//cout << "ur1 to clean cell " << c4 + 1 << endl;
		wur.CleanCell(c4, wur.digs);
		return;
	}
	int rowa = c3 / 9, cola = c3 % 9, boxa=3*(rowa/3)+cola/3;
	int rowb = c4 / 9, colb = c4 % 9, boxb = 3 * (rowb / 3) + colb / 3;

	BF128 wab; wab.SetAll_0();
	if (rowa == rowb)wab |= units3xBM[rowa];
	if (cola == colb)wab |= units3xBM[cola+9];
	if (boxa == boxb)wab |= units3xBM[boxa+18];
	SLG slg; slg.InitFromSolve();
	// now kill cell c4 and c3 dummy no digit pair 
	slg.sv.cells[c3] |= slg.sv.cells[c4];// all digits
	slg.sv.cells[c3] &= ~wur.digs; // kill 2 digits 
	wab.Clear_c(c4);
	wab &= sv81w.unsolved_cells;
	BF128 x = wab;
	int cell;
	while ((cell = x.getFirstCell()) >= 0) {
		x.Clear_c(cell);
		slg.tsc[slg.ntsc++] = cell;
	}
	// build linkfield all unit(s) less biv in c3 c4
	//memset(slg.lfield, 0, sizeof slg.lfield);
	memset(slg.orf, 0, sizeof slg.orf);
	for (int idig = 0; idig < 9; idig++) {
		BF128 w = slg.sv.dm[idig] & wab;
		if (idig == wur.d1 || idig == wur.d2)w.Clear_c(c3);
		slg.lfield[idig] = w;
	}
	int more = slg.Expand_sc_ld();// now expand/clean
	/*
		if (opp & 4 && more) 
			cout << "active  unit " << iu << endl;
	*/
}

//______________ solve band/stack cells as set
int SOLVE::IsBandStack() {// Solve all cells  as sets
	if (opp & 8) cout << "entry solve band/stack" << endl;
	int iret = 0; 
	for (int ibs = 0; ibs < 6; ibs++) {
		SLG slg; slg.InitFromSolve();
		memset(slg.orf, 0, sizeof slg.orf);
		BF128 cfield = sv81w.unsolved_cells & band3xBM[ibs];
		// store sets by size
		for (int ic = 0; ic < 4; ic++) {// sets by size 
			BF128 x = cfield & sets.c2345[ic];
			int cell;
			while ((cell = x.getFirstCell()) >= 0) {
				x.Clear_c(cell);
				slg.tsc[slg.ntsc++] = cell;
			}
		}
		// link field per digit
		for (int i = 0; i < 9; i++)
			slg.lfield[i] = sv81w.dm[i] & cfield;
		int more = slg.Expand_sc_ld();
		iret += more;
		if (opp & 8 && more) {
			cout << "active band/stack " << ibs << endl;			
		}
	}
	return iret;
}
int SOLVE::IsBandStack23(int serate ) {// Solve all cells2/3v as sets
	if (opp & 8) cout << "entry solve band/stack 23" << endl;
	int iret = 0;
	for (int ibs = 0; ibs < 6; ibs++) {
		SLG slg; slg.InitFromSolve();
		memset(slg.orf, 0, sizeof slg.orf);
		BF128 cfield = sv81w.unsolved_cells & band3xBM[ibs];
		// store sets pairs triplets  by size
		for (int ic = 0; ic < 2; ic++) {// sets by size 
			BF128 x = cfield & sets.c2345[ic];
			int cell;
			while ((cell = x.getFirstCell()) >= 0) {
				x.Clear_c(cell);
				slg.tsc[slg.ntsc++] = cell;
			}
		}
		if (slg.ntsc < 3) return 0;
		// link field per digit
		for (int i = 0; i < 9; i++)
			slg.lfield[i] = sv81w.dm[i] & cfield;
		int more = slg.Expand_sc_ld(serate);
		if (more)iret |= 1 << ibs;
		if (opp & 8 && more) {
			cout << "active band/stack " << ibs << endl;
		}
	}
	return iret;
}
