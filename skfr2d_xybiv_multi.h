#pragma once

/*
* Multi chains
 this is a group of patterns based on one of the basic sets, 
 cell set or digit set and bi value AICs 

 The general pattern is for a set a,b,..n

 a  {AIC} x
 b  {AIC} x
 ...
 n  {AIC} x

 where x is the target candidate not valid
 one or more of the set members can have no AIC (see directly the target)  
 ______________________________________  
kraken cells (cell >= 3 digits )
a,b,..n are the digits of the set

can have one digit(a) with direct view in the target, then (a=x)

n-1 biv seen or n biv seen first test
all cell >= 3 digits with n-1 or n biv seen
______________________________
"kraken" digit in unit 
a,b,...n are the cells of the set
steps[] = { 4, 6, 8, 12,
here up to 3 cells (mini row/mini column) can see the target
*/

struct ER7MUL {
	DM9 start_cands,*curexpand;
	struct CELLSTOSEE {
		int c, d_no_biv, nmin;
		inline void Add(int ce, int de, int n) {
			c = ce; d_no_biv = de, nmin = n;
		}
	}c_to_see[50];
	struct DUSTOSEE {
		int d,u, u_no_biv, nmin;
		inline void Add(int de, int ue,int unb, int n) {
			d = de; u = ue; u_no_biv = unb, nmin = n;
		}
	}du_to_see[100];
	struct XYSM {
		CDPM cdpm;
		DM9 pmseen;
		CAND cand1, cand2;
		int ispot;
		void Init(XYSM& o) {
			*this = o;	ispot++;
			cdpm.Add(cand1); cdpm.Add(cand2);
		}
		void DoStep();
		void DoNewD(int da, int ca);
	}xysm[20];

	struct CAND_VIEW {
		BF128 cbiv, dbiv;
		int dig, cell;
	}cview[200];
	SOLV81* ps;
	int nc_to_see, ndu_to_see,cur_d,cur_c,cur_u;
	int dbus[9];// digit biv units 
	void Init();// find sets of interest
	void AddCells(BF128 x);
	void AddDU(int s1, int s2);
}er7mul;


int SOLV81::DoEr80() {//multi chains 
	cout << "entry new DoEr80()  "  << endl;
	int iret = 0;
	er7mul.Init();
	return 0;

}

void ER7MUL::AddCells(BF128 cw) {
	DM9 wdm9;
	int c, dnobiv = -1;
	while ((c = cw.getFirstCell()) >= 0) {
		cw.Clear_c(c);
		int v = ps->cells[c], nd = _popcnt32(v),
			nobiv = 0, x = v, d, bit;
		wdm9.Init();
		while (x && nobiv < 2) {
			bitscanforward(d, x); bit = 1 << d; x ^= bit;
			BF128 wd = ps->dm[d] & cell_z3x[c],
				wdb = wd & (xybiv.cset_d[d] | xybiv.dig_nodes[d]);
			if (wdb.isEmpty()) { nobiv++; dnobiv = d; continue; }
			wdm9.Ordx(d, wdb);
		}
		if (nobiv > 1)continue;
		start_cands.Orx(wdm9);
		cout << cell_names[c] << " to see nobiv=" << nobiv << " nd=" << nd << endl;
		c_to_see[nc_to_see++].Add(c, dnobiv, 4 * nd - 2 * nobiv);
	}
}
void ER7MUL::AddDU(int s1, int s2) {
	DM9 wdm9;
	for (int idig = 0; idig < 9; idig++) {
		DSETS& ds = solve.sets.ds[idig];
		uint32_t* ds234m = ds.d234m.bf.u32, iu, c,	dsw = 0;
		for (int i = s1; i <= s2; i++)dsw |= ds234m[i];
		if (dsw)			cout << Char27out(dsw) << " sets for digit " << idig + 1;
		while (dsw) {
			bitscanforward(iu, dsw); dsw ^= (1 << iu);
			BF128 wdu = ps->dm[idig] & units3xBM[iu], x = wdu;
			wdm9.Init();
			int nobiv = 0, unit_nobiv = 0777777777, nc = x.Count96();
			while ((c = x.getFirstCell()) >= 0) {
				x.Clear_c(c);
				BF128 wd = ps->dm[idig] & cell_z3x[c],
					wdb = wd & (xybiv.cset_d[idig] | xybiv.dig_nodes[idig]);
				if (wdb.isEmpty()) {
					nobiv++; unit_nobiv &= tcellsrcb[c];	continue;
				}
				wdm9.Ordx(idig, wdb);
			}
			if (!unit_nobiv) continue;
			start_cands.Orx(wdm9);
			cout << " d " << idig + 1 << " unit " << iu + 1 << " to see nobiv=" << nobiv << " nc=" << nc << endl;
			du_to_see[ndu_to_see++].Add(idig, iu, unit_nobiv, 4 * nc - 2 * nobiv);
		}
	}
}
void ER7MUL::Init() {
	DM9 wdm9;
	ps = &solve.sv81w;
	nc_to_see= ndu_to_see=0;	
	start_cands.Init();	
	// load unit biv per digit
	for (int idig = 0; idig < 9; idig++) {
		DSETS& ds = solve.sets.ds[idig];
		dbus[idig] = ds.d234m.bf.u32[0];
	}

	// find cell to see not biv, link to biv less 0/1 digit
	BF128 cw = ps->unsolved_cells - ps->ccm[1];// no cell biv to see
	AddCells(cw); // all cells > biv
	AddDU(1, 3);//all sets > biv

	
}


void ER7MUL::XYSM::DoStep() {
	SOLV81 & p = *er7mul.ps;
	Init(*(this - 1));
	if (ispot > 10) return;// safety
	int d = cand2.Digit(), c = cand2.Cell(), c1 = cand1.Cell();
	BF128 wd = (p.dm[d] & cell_z3x[c]),
		wdc = wd & p.ccm[1],
		wdd = wd & xybiv.dig_nodes[d];
	er7mul.curexpand->Ordx(d, wd);// update seen
	// new  digit biv seen 
	if (wdd.isNotEmpty()) {
		int c2;
		while ((c2 = wdd.getFirstCell()) >= 0) {
			wdd.Clear_c(c2);
			DoNewD(d, c2);// Find the bi value
		}
	}
	// new cell  biv seen 
	if (wdc.isNotEmpty()) {
		int c2,d2;
		while ((c2 = wdc.getFirstCell()) >= 0) {
			wdc.Clear_c(c2);
			// New digit
			{
				register int v = p.cells[c2]; v &= ~(1 << d);
				bitscanforward(d2, v);
			}
			cand1.Set(d, c2); cand2.Set(d2, c2);
			if (cdpm.On(cand1) || cdpm.On(cand2)) continue;
			(this + 1)->DoStep();
		}
	}
	// if old not cell biv  can also be a cell new digit bi value 
	if (c != c1 && xybiv.dig_m.On_c(c)) {
		int v = xybiv.sv.cells[c], d2; v ^= 1 << d;
		while (v) {
			bitscanforward(d2, v);	v ^= 1 << d2;
			if (xybiv.dig_nodes[d2].Off_c(c)) continue;
			DoNewD(d2, c);
		}
	}
}

void  ER7MUL::XYSM::DoNewD(int da, int ca) {
	cand1.Set(da, ca); if (cdpm.On(cand1)) return;
	SOLV81& p = *er7mul.ps;
	int c;
	BF128 w = (p.dm[da] & xybiv.dig_nodes[da]) & cell_z3x[ca];
	NameBf128List(" cells to see for biv ", w);
	while ((c = w.getFirstCell()) >= 0) {
		w.Clear_c(c);
		cand2.Set(da, c);	if (cdpm.On(cand2)) continue;
		int units = tcellsrcb[ca] & tcellsrcb[c] & er7mul.dbus[da];
		if (units) //it is a bi value, do next step
			(this + 1)->DoStep();		
	}
}
