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
	char* Out(char* ws) {// ws must be length >=6
		ws[5] = 0;
		ws[0] = (char)(Digit()+'1');
		memcpy(&ws[1], cell_names[Cell()], 4);
		return ws;
	}
};
class DM9 {// pm to store a property
	BF128 dm[9];
public:
	inline void Init() { memset(dm, 0, sizeof dm);}
	inline void Set(int d, int c) { dm[d].Set_c(c); }
	inline int On(int d, int c) { return dm[d].On_c(c); }
	inline int Off(int d, int c) {return dm[d].Off_c(c);}
	inline BF128 Getd(int d) { return dm[d]; }
	void Orx(DM9& o) {
		for (int i = 0; i < 9; i++)dm[i] |= o.dm[i];
	}
	inline void Ordx(int d,BF128& x) {
		dm[d] |=x;
	}
};
class CDPM {// candidate PM for expand
	BF128 dm[9], crm[9];
public:
	inline void Add(CAND& cd) {
		dm[cd.Digit()].Set_c(cd.Cell());	}
	inline void Add(int d,int c) {
		dm[d].Set_c(c);
	}
	inline int On(CAND& cd) {
		return dm[cd.Digit()].On_c(cd.Cell());	}
	inline int Off(CAND& cd) {
		return dm[cd.Digit()].Off_c(cd.Cell());
	}
};


struct XYBIV {// sets links group for bi values
	struct SDB {
		BF128 dbf;
		int digit, unit, c1, c2;
		int Set(BF128& bf, int d, int iu) {
			dbf = bf; digit = d, unit = iu;
			c1 = dbf.getFirstCell(); c2 = dbf.getLastCell();
			if (iu < 18) return 1;
			// check redundancy
			int uus = tcellsrcb[c1] & tcellsrcb[c2];
			if ((uus != (1 << iu)) )return 0;// redundant
			return 1;
		}
		void Out1() {
			cout << digit + 1 << " " << cell_names[c1] << " "
				<< cell_names[c2] << endl;
		}
	}sdb[200];
	struct XYS {
		CDPM cdpm;
		CAND cand1, cand2;
		int ispot;
		void Init(XYS& o) {
			*this = o;	ispot++;
			cdpm.Add(cand1); cdpm.Add(cand2);
		}
		void DoStep();
		int DoNewDC(int d, int c);// new digit in cell not biv
		int DoNewD(int d, BF128 w);// new digit biv
		int DoNewDcom(int da, int ca );// new digit bivalue after first
		int DoNewC(int d, BF128 w);// nex cell biv
	 }xys[20];

	struct CSPOT {
		BF128 ass;// 
		uint32_t ufree[9]; // free units for digit init to all 1 
		int ispot,cell, tdig[2],idig , dig;
		char vass[81];
		void Init();
		void SetSpot(int c,int i);
		int GetNextd() {
			if (idig > 1) return 1;
			dig = tdig[idig++];
			register int a= tcellsrcb[cell],
				b=a& ufree[dig]; 
			if(a!=b)return GetNextd();
			vass[cell] = dig;
			ass.Set_c(cell);
			// setup next
			CSPOT* sn = this + 1;
			memcpy(sn->ufree, ufree, sizeof ufree);
			memcpy(sn->vass,vass, 81);
			sn->ass = ass;
			sn->ufree[dig] &= ~a;
			sn->idig = 0;
			return 0;
		}
		void Out1(int cpt=0) {
			cout << cpt << " got a valid csets" << endl;
			NameBf128List("cells", ass);
			cout << "digs  ";
			for (int i = 0; i < 81; i++) {
				int d = vass[i];
				if (d < 10)cout << d + 1 << "    ";
			}
			cout<< endl;
		}
	}cspot[60];	// cells sets 
	struct DSPOT {
		BF128 ass, dcells;  
		uint32_t ufree[9]; // free units for digit init to all 1 
		int ispot, dig, tcell[2], icell, cell;
		void Init(SDB& sa, int i) {
			ispot = i;
			tcell[0] = sa.c1; tcell[1] = sa.c2;
			dig = sa.digit;
			dcells = sa.dbf;
		}
		char vass[81];
		void Init(CSPOT & sc) {
			ass = sc.ass;
			memcpy(vass, sc.vass, 81);
			memcpy(ufree, sc.ufree, sizeof ufree);
			icell = 0;
		}
		int GetNextCell();
		void Assign(int c ) {
			//cout << ispot << "assign dspot " << dig + 1 << cell_names[c ] << endl;
			cell = c;
		}
		void Next() {
			DSPOT* sp = this - 1;
			ass = sp->ass;
			memcpy(vass, sp->vass, 81);
			memcpy(ufree, sp->ufree, sizeof ufree);
			int c = sp->cell;
			if (c >= 0) {
				ufree[sp->dig] &= ~tcellsrcb[c];
				ass.Set_c(sp->cell);
				vass[c] = sp->dig;
			}
			icell = 0;
		}
		void Out1(int cpt = 0) {
			NameBf128List("cells", ass);
			cout << "digs  ";
			for (int i = 0; i < 81; i++) {
				int d = vass[i];
				if (d < 10)cout << d + 1 << "    ";
			}
			cout << endl;
		}
		void OutFree() {
			cout << "free status" << endl;
			for (int i = 0; i < 9; i++)
				cout << Char27out(ufree[i]) << " dig "<< i + 1 << endl;
		}
	}dspot[200];
	BF128 csets,cset_d[9] , dig_nodes[9],dig_m ;
	int nsdb, ncsets;
	int iret,elimdone,nlim,rating,dtarget,ctarget,edebug;
	//============================ list of sets/links
	BF128 lfield[9], orf[9], andf[9], allcells, cellslinks;
	BF128 rclean1[9];// back clean in serate mode 

	SOLV81  sv,*p;	

	//=============== elim and error starts
	CAND cand_elim;
	int celim_d, celim_c;
	int i2,// choices for the 2 starts
		tcb[5], ntcb,		// cells bivalues
		td0[5], ntd0,// digit biv in cell
		tdbs[5], ntdbs;// digit starts seen
	CAND st_cand1, st_cand2;
	int st_d1, st_c1, st_d2, st_c2, end_d1, end_c1, end_d2, end_c2;

	void AddClink(BF128 *bfd, int c, int d) {
		int v = p->cells[c] & ~(1 << d),dig;
		while (v) {
			bitscanforward(dig, v);
			v ^= 1 << dig;
			bfd[dig].Set_c(c);
		}
	}
	int IsNotBiv(int d, int c1, int c2) {
		// must be bi value in unit
		int units = tcellsrcb[c1] & tcellsrcb[c2], ua, ub;
		bitscanforward(ua, units); bitscanreverse(ub, units);
		int na = (sv.dm[d] & units3xBM[ua]).Count96(),
			nb = (sv.dm[d] & units3xBM[ub]).Count96();
		if (na != 2 && nb != 2) return 1;
		return 0;
	}
	BF128 GetBiv(int d, int c) {
		BF128 w,wd=sv.dm[d]&cell_z3x[c]; 
		w.SetAll_0();
		for (int i = 0; i < 3; i++) {
			int u = tcellsrcb3[c][i];
			BF128 wdu = wd & units3xBM[u];
			if (wdu.Count96() == 1) w |= wdu;
		}
		return w;
	}
	int Init();	
	int Er71();
	void Er71B(int cpt);// after a valid perm for cells 
	int Er71C();// after all perms done
	void Er7a(int i); // start is another digit in cell
	void Er7_b(int i); // end digit biv
	void Er7_c(int i); // end cell biv
	void Er7b(int i); // start is digit bi value
	void Er7c(int i); // start is cell bi value
	void GoAIC();
	void DoElims(int ispot);
	void NewSol(char* zs,int cpt=0) {
		BF128 wor[9]; memset(wor,0,sizeof wor);
		for (int i = 0; i < 81; i++) {
			int d = zs[i];		if (d > 8)continue;
			//if (cpt < 5)
				//cout << "new " << d + 1 << cell_names[i] << endl;
			AddClink(wor, i, d);// cell killed
			wor[d] |= cell_z3x[i];
		}
		for (int i = 0; i < 9; i++) {
			orf[i] |= lfield[i] & wor[i];
			andf[i]&= wor[i];
		}
	}
	int Elims() {
		int aig = 0;
		if(0)for (int i = 0; i < 9; i++) {
			lfield[i] -= orf[i];
			if (lfield[i].isNotEmpty()) {
				cout << " assigned  for digit " << i + 1;
				NameBf128List(" ", lfield[i]);
			}
		}
		for (int i = 0; i < 9; i++) {
			if (andf[i].isNotEmpty()) {
				aig = 1;
				//cout << " elims  for digit " << i + 1;
				//NameBf128List(" ", andf[i]);
			}
		}		
		return  aig;
	}
	void StatusBiv() {
	//	BF128 csets,cset_d[9] , dig_nodes[9],dig_m ;
		cout << "cset dig nodes status" << endl;
		char ws[82]; ws[81] = 0;
		cout << csets.String3X(ws) << " csets" << endl;
		cout << dig_m.String3X(ws) << " dig_m" << endl << endl;

		for(int i = 0; i < 9;i++) if(cset_d[i].isNotEmpty())
			cout << cset_d[i].String3X(ws) << " d " << i+1 << endl;
		
		cout << "dig nodes" << endl;
		for (int i = 0; i < 9; i++) if (dig_nodes[i].isNotEmpty())
			cout << dig_nodes[i].String3X(ws) << " d " << i + 1 << endl;
	}
}xybiv;
void XYBIV::CSPOT::Init() {
	memset(ufree, 255, sizeof ufree);
	memset(vass, 10, 81);
	ass.SetAll_0();
	idig = 0;
}
void XYBIV::CSPOT::SetSpot(int c,int i) {// c is a bi value
	ispot = i;
	cell = c;
	int digs = xybiv.sv.cells[c];
	bitscanforward(tdig[0], digs);
	bitscanreverse(tdig[1], digs);
	//cout << ispot << cell_names[c] << " " << tdig[0] + 1 << tdig[1] + 1 << endl;
}
int  XYBIV::Init() {
	int debug =0;
	if(debug)cout << "XYBIV Init" << endl;
	p=&solve.sv81w;
	sv = *p;
	// get sets cell and map per digit 
	{
		csets = solve.sets.c2345[0];// set bi values 
		for (int idig = 0; idig < 9; idig++)
			cset_d[idig] = csets & sv.dm[idig];
	}
	nsdb =  0;
	memset(dig_nodes, 0, sizeof dig_nodes);
	// get digits bi values as sets 
	for (int idig = 0; idig < 9; idig++) {
		DSETS& ds = solve.sets.ds[idig];
		register uint32_t ux = ds.d234m.bf.u32[0], u;// digit biv units
		//cout << Char27out(ux) << "  units biv for digit " << idig + 1 << endl;
		while (ux) {
			register SDB& s = sdb[nsdb];
			bitscanforward(u, ux);
			ux ^= 1 << u;// clear bit
			BF128 y= ds.rcb[u];
			if (s.Set(y, idig, u)) 	nsdb++;
			else continue;			
			dig_nodes[idig] |= y; 
		}
	}
	// find cells multiple digits biv
	{
		BF128 r1 = dig_nodes[0], r2; r2.SetAll_0();
		for (int i = 1; i < 9; i++) {
			r2 |= r1 & dig_nodes[i]; r1 |= dig_nodes[i];
		}
		dig_m = r2;
	}
	// find digits to erase and digit biv to ignore
	{
		BF128 wm = csets | dig_m;
		for (int i = 0; i < 9; i++) {
			BF128 wd = dig_nodes[i];	if (wd.isEmpty()) continue;
			if ((wd & wm).isEmpty()) {
				//cout << " can erase dig bivs " << i + 1 << endl;
				dig_nodes[i].SetAll_0();
				for (int i2 = 0; i2 < nsdb; i2++)
					if (sdb[i2].digit == i)
						sdb[i2].unit = -1;// flag to erase later 
				continue;
			}

		}

	}

	//StatusBiv();
	// try to find a 70 cycle potential
	{
		for (int i2d = 0; i2d < 36; i2d++) {
			int fl = floors_2d[i2d], d1, d2;
			bitscanforward(d1, fl); bitscanreverse(d2, fl);
			BF128 d2m = dig_nodes[d1] & dig_nodes[d2],
				c2m = cset_d[d1] & cset_d[d2];
			d2m -= c2m;// multiple not bi value
			if (d2m.isEmpty())continue; if (c2m.isEmpty())continue;
			//cout << "try " << d1 + 1 << d2 + 1;
			//NameBf128List(" M cells ", d2m);
			BF128 x = d2m;
			int cell1, cell2;
			while ((cell1 = x.getFirstCell()) >= 0) {
				x.Clear_c(cell1);
				BF128 y = c2m - cell_z3x[cell1];// biv not seen by cell1
				BF128 xd1 = GetBiv(d1,cell1),	xd2 = GetBiv(d2, cell1);

				if (y.isEmpty())continue;
				while ((cell2 = y.getFirstCell()) >= 0) {
					y.Clear_c(cell2);
					// cell2 must see both biv from cell1
					BF128 xyd1 = xd1 & cell_z3x[cell2],
						xyd2 = xd2 & cell_z3x[cell2];
					if (xyd1.isEmpty())continue; if (xyd2.isEmpty())continue;
					int cell3 = xyd1.getFirstCell(), cell4 = xyd2.getFirstCell();
#ifdef SEROUT
					cout << "ER70 loop do cleaning "
						<<cell_names[cell1]<<" " << cell_names[cell3] 
						<< " " << cell_names[cell2] << " " << cell_names[cell4] << endl;
#endif			
					int v = p->cells[cell1];
					v &= ~((1 << d1) | (1 << d2));
					p->CleanCell(cell1, v);
					BF128 w = (sv.dm[d1] & cell_z3x[cell2]) & cell_z3x[cell3];
					p->Clean(d1, w);
					w = (sv.dm[d2] & cell_z3x[cell2]) & cell_z3x[cell4];
					p->Clean(d2, w);				
					serate.SetRating(70);
					return 1;
				}
			}
		}

	}	
	return 0;
}
int ctl = 0;
int XYBIV::Er71() {
	//cout << "entry Er71 xbiv" << endl;
	sv= solve.sv81w;
	p = &solve.sv81w;
	memset(lfield, 0, sizeof lfield);
	memset(orf, 0, sizeof orf);
	// setup linkfield
	{
		BF128 wun = p->unsolved_cells;
		for (int i = 0; i < ncsets; i++) {
			CSPOT& s = cspot[i];
			BF128 w = wun & cell_z3x[s.cell];
			int d = s.tdig[0];		lfield[d] |= w & p->dm[d];
			d = s.tdig[1];		lfield[d] |= w & p->dm[d];

		}
		for (int i = 0; i < nsdb; i++) {
			SDB& s = sdb[i];
			int d = s.digit;
			BF128 w = wun & p->dm[d];
			lfield[d] |= w& cell_z3x[s.c1];
			lfield[d] |= w & cell_z3x[s.c2];
			// add c1,c2 as links
			AddClink(lfield, s.c1, d); AddClink(lfield, s.c2, d);
		}
		memcpy(andf, lfield, sizeof andf);
		if (0) {
			cout << "lfield status" << endl;
			char ws[82]; ws[81] = 0;
			for (int i = 0; i < 9; i++)if (lfield[i].isNotEmpty())
				cout << lfield[i].String3X(ws) << " d=" << i+1 << endl;
		}
	}
	// inital for spots cells (can be 0)
	{
		cspot[0].Init();
		ncsets = csets.Count96();
		BF128 x = csets;
		for (int i = 0; i < ncsets; i++) {
			CSPOT& s = cspot[i];
			int c = x.getFirstCell();
			x.Clear_c(c);
			s.SetSpot(c,i);
		}
	}
	// inital for spots digits
	{

		for (int i = 0; i < nsdb; i++) {
			SDB& sa = sdb[i];
			DSPOT& s = dspot[i];
			s.Init(sa, i);

		}
	}
	// find all valid csets
	{
		int cpt=0;
		CSPOT* sw = cspot;
	iwsloop:
		if (sw->GetNextd()) {
			if (sw == cspot) return Er71C(); // closed
			sw--; goto iwsloop;
		}
		if (sw->ispot >= (ncsets - 1)) {
			Er71B(++cpt);
			goto iwsloop;// same spot
		}
		sw ++;	
		goto iwsloop;
	}
	return 0;

}
int XYBIV::DSPOT::GetNextCell() {
	if (icell > 1) return 1;
	if (0) {
		cout << Char27out(ufree[dig]) << "dget  spot " << ispot
			<< " icell=" << icell << " " << cell_names[tcell[0]] << " " << cell_names[tcell[1]];
		NameBf128List(" dcells ", dcells);
	}
	cell = -1;//flag as  not assigned in this spot
	if (icell) {// assign second cell if valid
		icell++;
		register int r = tcellsrcb[tcell[1]];
		if ((r & ufree[dig]) != r)return 1;//closed for this spot
		Assign(tcell[1]); return 0;
	}
	BF128 w = dcells & ass;
	//NameBf128List(" dcells & ass ", w);
	//assign first if not hit 
	if ((dcells & ass).isEmpty()) {
		icell++;
		register int r = tcellsrcb[tcell[0]];
		if ((r & ufree[dig]) != r)return GetNextCell();
		Assign(tcell[0]); return 0;

	}
	// now hit once or twice see if ok ...
	icell = 2; //finished
	if ((dcells & ass) == dcells) {// 2 hits valid or dead
		if (vass[tcell[0]] == dig || vass[tcell[1]] == dig)
			return 0; else return 1;
	}
	// one hit if !ok assign the second
	int cellb = (dcells & ass).getFirstCell();

	if (vass[cellb] == dig) return 0;
	int cell2 = (tcell[0] == cellb) ? tcell[1] : tcell[0];
	//cout << cell_names[cell2] << " " << cell_names[cellb] << " " << cell_names[tcell[0]]
	//	<< " " << cell_names[tcell[1]] <<  endl;
	register int r = tcellsrcb[cell2];
	if ((r & ufree[dig]) != r)return 1;
	Assign(cell2); return 0;
}
void XYBIV::Er71B(int cpt) {
	//if (cpt < 77)return;
	//if (cpt != 78)return;
	CSPOT& s = cspot[ncsets];
	//s.Out1(cpt);// valid perm print
	// find all valid dsets
	{
		DSPOT* sw = dspot;
		sw->Init(s);	//	sw->OutFree();
	iwsloop:
		if (sw->GetNextCell()) {
			if (sw == dspot) return ; // closed
			sw--; goto iwsloop;
		}
		(sw+1)->Next();
		if (sw->ispot >= (nsdb - 1)) {
			//cout << "newsol" << endl;
			NewSol((sw + 1)->vass,++ctl);
			goto iwsloop;// same spot
		}
		sw++; 
		goto iwsloop;
	}
}
int XYBIV::Er71C() {
	if(! Elims()) return 0;// check (and print) elims to search
	rating = 71, elimdone = 0; nlim = 2;
	int ca;
	for (int i = 0; i < 9; i++) {// check each digit 
		int stopifelim;
		celim_d = i;
		BF128 x = andf[i],wd=dig_nodes[i]|cset_d[i];
		if (x.isEmpty()) continue;
		while ((ca = x.getFirstCell()) >= 0) {
			edebug = 0;
			//if (i == 6 && ca == 74) edebug = 2;// 7 r9c3
			if (edebug) cout << "debug " << edebug << " for " << i + 1 << cell_names[ca] << endl;
			x.Clear_c(ca);
			stopifelim = 0;
			if(csets.On_c(ca))stopifelim = 1;
			if (dig_nodes[i].On_c(ca))stopifelim = 1;

			celim_c = ca;
			cand_elim.Set(i, ca);
			iret = 0;
			BF128 wda = wd & cell_z3x[ca],wda2=wda;
			if (wd.On_c(ca))wda2.Set_c(ca);
			if (edebug) {
				cout << i + 1 << cell_names[ca];
				NameBf128List(" seen ", wda2);
			}

			ntd0  = 0;
			int v = sv.cells[ca]^(1<<i),dd;// other digits
			while (v) {// start dig biv in cell
				bitscanforward(dd, v); v ^= (1 << dd);
				if (dig_nodes[dd].On_c(ca)) td0[ntd0++] = dd;
			}
			BF128 w = dig_nodes[i] & cell_z3x[ca];
			ntdbs=w.Table3X27(tdbs);// starts digit bi values
			w=cset_d[i] & cell_z3x[ca];
			ntcb= w.Table3X27(tcb);// starts cells bi values
			int ntot = ntd0 + ntdbs + ntcb;
			if (ntot < 2)continue;
			// try all possible 2 starts
			if(0)cout << celim_d + 1 << cell_names[celim_c] << "   "
				<< ntot << " a " << ntd0 << " b " << ntdbs << " c " << ntcb 
				<< " stopifelim "<< stopifelim << endl;
			for (int i1 = 0; i1 < ntd0; i1++) Er7a(i1); // one other dig in  in cell
			if (iret)if (stopifelim)break; else continue;
			for (int i1 = 0; i1 < ntdbs; i1++)Er7b(i1); // one digit biv
			if (iret)if (stopifelim)break; else continue;
			for (int i1 = 0; i1 < ntcb - 1; i1++) Er7c(i1);// one cellt biv
			if (iret)if (stopifelim)break; else continue;
		}
		if (iret)if (stopifelim)break;
	}
	if(elimdone)serate.SetRating(71);
	return elimdone;
}
//==== 2 AIC starts/end
void XYBIV::Er7a(int i) {// start another digit in cell
	if (edebug)cout << "er7a " << i << endl;
	st_d1 = st_d2 = td0[i];// is a digit  bivalue start
	st_c1 = celim_c;
	BF128 w = dig_nodes[st_d1] & cell_z3x[celim_c];// usually one cell
	if (edebug) {
		cout << " c1 " << cell_names[st_c1];
		NameBf128List(" st_c2list  ", w);
	}
	while ((st_c2 = w.getFirstCell()) >= 0) {
		w.Clear_c(st_c2);
		if (IsNotBiv(st_d1, st_c1, st_c2)) continue;
		if (edebug) 	cout << " c2 " << cell_names[st_c2] << endl;;
		for (int i2 = i + 1; i2 < ntd0; i2++) {// second  other dig in  in cell
			end_d1 = end_d2 = td0[i2];
			end_c1=st_c1;
			BF128 w2 = dig_nodes[end_d1] & cell_z3x[celim_c];// usually one cell
			while ((end_c2 = w2.getFirstCell()) >= 0) {
				w2.Clear_c(end_c2);
				if (IsNotBiv(end_d1, end_c1, end_c2)) continue;
				GoAIC();
			}
		}
		for (int i2 = 0; i2 < ntdbs; i2++)Er7_b(i2); // second  digit biv		
		for (int i2 = 0; i2 < ntcb; i2++)Er7_c(i2); // second  cell biv
	}
}
void XYBIV::Er7_b(int i) {//end digit bi value not elim not first
	//cout << "er7_b " << i << endl;
	end_c1 = tdbs[i];// first cell of the bivalue for digit celim_d
	if (edebug)cout << "er7_b " << i << "end_c1 " << cell_names[end_c1] << endl;
	end_d1 = end_d2 = celim_d;
	BF128 w2 = dig_nodes[end_d1] & cell_z3x[end_c1];// usually one cell
	w2.Clear_c(celim_c); w2.Clear_c(st_c1); w2.Clear_c(st_c2);
	while ((end_c2 = w2.getFirstCell()) >= 0) {
		w2.Clear_c(end_c2);
		if (IsNotBiv(end_d1, end_c1, end_c2)) continue;
		GoAIC();
	}
}
void XYBIV::Er7_c(int i) {//end cell bi value
	//cout << "er7_c " << i << endl;
	end_c1 = end_c2=tcb[i];
	if (edebug)cout << "er7_c " << i << "end_c1 "<< cell_names[end_c1] 
		<< " st_c1 " << cell_names[st_c1] << " st_c2 " << cell_names[st_c2] << endl;
	if (end_c1 == st_c1 || end_c1 == st_c2) return;
	end_d1= celim_d;
	int v = sv.cells[end_c1] ^ (1 << end_d1);
	if (edebug)cout << Char9out(v) << " v" << endl;
	if (!v)return;// safety against redundancy
	bitscanforward(end_d2, v);
	if (edebug)cout << "er7_c go"   << endl;
	GoAIC();
}
void XYBIV::Er7b(int i) {//start digit bi value not elim
	if(edebug)cout << "er7b " << i << endl;
	st_c1 = tdbs[i];// first cell of the bivalue for digit celim_d
	st_d1 = st_d2 = celim_d;
	BF128 w2 = dig_nodes[st_d1] & cell_z3x[st_c1];// usually one cell
	w2.Clear_c(celim_c);
	if (edebug) {
		cout << " c1 " << cell_names[st_c1];
		NameBf128List(" st_c2list  ", w2);
	}
	while ((st_c2 = w2.getFirstCell()) >= 0) {
		w2.Clear_c(st_c2);
		if (IsNotBiv(st_d1, st_c1, st_c2)) continue;
		/*
		// must be bi value in unit
		int units = tcellsrcb[st_c1] & tcellsrcb[st_c2], ua, ub;
		bitscanforward(ua, units); bitscanreverse(ub, units);
		int na = (p->dm[st_d1] & units3xBM[ua]).Count96(),
			nb = (p->dm[st_d1] & units3xBM[ub]).Count96();
		if (na != 2 && nb != 2) continue;
		*/
		for (i2 = i + 1; i2 < ntdbs; i2++)Er7_b(i2); // second  digit biv		
		for (int i2 = 0; i2 < ntcb; i2++)Er7_c(i2); // second  cell biv
	}
}

void XYBIV::Er7c(int i) {//start  cell bi value
	//cout << "er7c " << i << endl;
	st_c1 = st_c2 = tcb[i];
	st_d1 = celim_d;
	int v = p->cells[st_c1] ^ (1 << st_d1);
	bitscanforward(st_d2, v);
	// look for a cell biv as end
	for (i2 = i + 1; i2 < ntcb; i2++) Er7_c(i2); // second  cell biv	
}
void XYBIV::GoAIC() {
	if (iret) return;// no rerundancy;
	if(edebug)
	cout << " goaic start " << st_d1+1 << cell_names[st_c1] 
		<< " " << st_d2+1 << cell_names[st_c2]
		<< " end " << end_d1+1 << cell_names[end_c1] 
		<< " " << end_d2+1 << cell_names[end_c2]
		<< endl;
	XYS* s = xys;
	memset(xys, 0, sizeof xys[0]);
	s->cand1.Set(st_d1, st_c1);
	s->cand2.Set(st_d2, st_c2);
	ctarget = end_c2; dtarget = end_d2;
	s->cdpm.Add(celim_d, celim_c);
	//cout <<" sss " << s->cand1.Out(cout1) << " " << s->cand2.Out(cout2) << endl;
	(++s)->DoStep();
}
void XYBIV::DoElims(int ispot) {
#ifdef SEROUT
	cout << " target reached ispot =" << ispot << " "
		<< celim_d + 1 << cell_names[celim_c] << "   ";
	for (int i = 0; i < ispot; i++) {
		XYS& s = xys[i];
		cout << "~" << s.cand1.Out(cout1) << "+" << s.cand2.Out(cout2) << "  ";
	}
	cout << "~" << dtarget + 1 << cell_names[ctarget];
	cout << "+" << end_d1 + 1 << cell_names[end_c1] << endl;
#endif
	iret = 1; elimdone++;
	p->Clear(celim_d, celim_c);
}



//==============  AICs search

void XYBIV::XYS::DoStep() {
	if (xybiv.iret)return;//finished
	int debug = xybiv.edebug;
	SOLV81& p = xybiv.sv;
	Init( *(this - 1));	
	if (ispot > xybiv.nlim) return;// safety
	int d = cand2.Digit(), c = cand2.Cell(),cx= cand1.Cell();
	// stop if target reached on
	if (d == xybiv.dtarget && c == xybiv.ctarget) return;	

	if (debug > 1)cout << "xy do step start ispot=" << ispot << " "
		<< cand2.Out(cout1)<< endl;
	BF128 wd = (p.dm[d] & cell_z3x[c]),
		wdc = wd & p.ccm[1],
		wdd=wd & xybiv.dig_nodes[d];
	// look for target reached
	if (d == xybiv.dtarget) {
		int ct = xybiv.ctarget;
		if (c == ct || (wdc | wdd).On_c(ct) ){
			xybiv.DoElims(ispot);
			return;
		}
	}
	if (debug > 1)cout << "DoNewDC(d, c)" << endl;
	if (DoNewDC(d, c)) return;// next digit bivalue starts  same cell
	if (debug > 1)cout << "DoNewD()" << endl;
	if (DoNewD(d, wdd)) return;// next possible bivalue same digit
	if (debug > 1)cout << "DoNewC()" << endl;
	if (DoNewC(d, wdc)) return;// next possible bi value cell
}
//____________ new biv is a digit biv start in last cell
int  XYBIV::XYS::DoNewDC(int d,int c) {
	int debug = xybiv.edebug;
	if(debug)cout << " DoNewDC " << d + 1 << cell_names[c] << endl;
	if (xybiv.dig_m.Off_c(c)) return 0;
	{// other digit biv same cell
		int v = xybiv.sv.cells[c], d2; v ^= 1 << d;
		if (debug) cout<<Char9out(v) << " v go new dc" << endl;
		while (v) {
			bitscanforward(d2, v);	v ^= 1 << d2;
			if (xybiv.dig_nodes[d2].Off_c(c)) continue;
			if (debug) cout << " call com  or target for d " << d2 + 1 << endl;
			// can be here the target 
			if (d2 == xybiv.dtarget && c == xybiv.ctarget) {
					xybiv.DoElims(ispot);		return 0;
			}
			DoNewDcom(d2, c);// Find the bi value
		}
	}	
	return 0;
}
//__________ new biv is a digit bi value
int  XYBIV::XYS::DoNewD(int d, BF128 w) {
	if (w.isEmpty()) return 0;
	//cout << " DoNewD " << d + 1 ;
	//NameBf128List(" for cells ", w);
	SOLV81& p = solve.sv81w;
	int c;
	while ((c = w.getFirstCell()) >= 0) {
		w.Clear_c(c); 
		DoNewDcom(d, c);// Find the bi value
	}
	return 0;
}
//__________ new digit  biv after first cell da ca 
int  XYBIV::XYS::DoNewDcom(int da,int ca) {
	int debug = xybiv.edebug;
	//cout << " DoNewDcom " << da + 1 << cell_names[ca] << endl;
	SOLV81& p = xybiv.sv;
	int c;
	BF128 w = (p.dm[da] & xybiv.dig_nodes[da]) & cell_z3x[ca];
	if(debug)NameBf128List(" cells to see for biv ", w);
	if (ispot >= xybiv.nlim) return 0;
	while ((c = w.getFirstCell()) >= 0) {
		w.Clear_c(c);
		cand1.Set(da, ca); cand2.Set(da, c);
		if (cdpm.On(cand1) || cdpm.On(cand2)) continue;
		if (xybiv.IsNotBiv(da, ca, c)) continue;
/*
		// must be bi value in unit
		int units = tcellsrcb[ca] & tcellsrcb[c],ua,ub;
		bitscanforward(ua, units); bitscanreverse(ub, units);
		int na = (p.dm[da] & units3xBM[ua]).Count96(),
			nb = (p.dm[da] & units3xBM[ub]).Count96();
		if (na != 2 && nb != 2) continue;
*/
		//cout << "go iret "<<xybiv.iret<< " ispot " <<ispot << endl;
		(this + 1)->DoStep();
	}
	return 0;
} 
//__________ new  cell BI VALUE
int XYBIV::XYS::DoNewC(int d,BF128 w) {
	if (w.isEmpty()) return 0;
	SOLV81& p = solve.sv81w;
	int c, d2;
	while ((c = w.getFirstCell()) >= 0) {
		w.Clear_c(c); //tcc.Set_c(c1);
		// New digit
		{
			register int v = p.cells[c]; v &= ~(1 <<d);
			bitscanforward(d2, v);
		}
		cand1.Set(d, c); cand2.Set(d2, c);
		if (ispot >= xybiv.nlim) return 0;
		if (cdpm.On(cand1) || cdpm.On(cand2)) continue;
		(this + 1)->DoStep();
	}
	return 0;
}
