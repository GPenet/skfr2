#pragma once


int cptgoaic = 0;

struct XYELIM {
	int ntot,stopifelim,// elim belongs to a bi value
		netbivcands,// same as ntot at the end count of bivcands
		ce_d,ce_c,// cand digit cell
		tcb[20], ntcb,		// cells bivalues
		td0[9], ntd0,// digit biv in cell
		tdbs[20], ntdbs;// digit starts seen
	CAND celim;
	BIVCANDS  etbivcands[30];
	inline void Init(CAND cd) {	celim = cd; ce_d = cd.Digit(); ce_c = cd.Cell();	}
	inline void Add0(int dd){ td0[ntd0++] = dd; }
	inline void Add(BF128 wdn, BF128 wcsd) {
		ntdbs = wdn.Table3X27(tdbs);// starts digit bi values
		ntcb = wcsd.Table3X27(tcb);// starts cells bi values
		ntot = ntd0 + ntdbs + ntcb;
	}
	void Status() {
		cout << "for elim " << ce_d + 1 << cell_names[ce_c] << "tot starts "
			<< ntot << " in cell " << ntd0 << " dig biv " << ntdbs << " cell biv " << ntcb
			<< " stopifelim " << stopifelim << endl;
		if (1) {
			int d1, c1, d2, c2;
			for (int i = 0; i < netbivcands; i++) {
				etbivcands[i].Get(d1, c1, d2, c2);
				cout << d1 + 1 << cell_names[c1] << "-" << d2 + 1 << cell_names[c2] << " ";
			}
			cout<< endl;
		}
		}

}txyelim[50],xyew;
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
		DM9 cdpm;
		BF128 used_cells;// check for no loop
		CAND cand1, cand2;
		int ispot;
		void Init(XYS& o) {
			*this = o;	ispot++;
			cdpm.Add(cand1); cdpm.Add(cand2);
		}
		void DoStep();
		int DoNewDC(int d, int c);// new digit in cell not biv
		inline int DoNewD(int d, BF128 w);// new digit biv
		inline int DoNewDcom(int da, int ca );// new digit bivalue after first
		inline int DoNewC(int d, BF128 w);// nex cell biv

	 }xys[21];

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
	BF128 all_target_cells;
	int nsdb, ncsets;
	int iret, stopifelim, // if elim, assign expected
		elimdone,myelimdone,// elim seen for one elim
		endelimdone,// elim seen with stop if elim
		rating,dtarget,ctarget,edebug,index_all_targets;
	//============================ list of sets/links
	BF128 lfield[9], orf[9], andf[9], allcells, cellslinks;
	BF128 rclean1[9];// back clean in serate mode 

	SOLV81  sv,*p;	
	//=============== elim and error starts
	CAND cand_elim;
	BIVCANDS bc1, bc2;
	int celim_d, celim_c,ielim,ielim_last;
	int i2,// choices for the 2 starts
		tcb[20], ntcb,		// cells bivalues
		td0[9], ntd0,// digit biv in cell
		tdbs[20], ntdbs;// digit starts seen
	CAND st_cand1, st_cand2;
	int st_d1, st_c1, st_d2, st_c2, end_d1, end_c1, end_d2, end_c2;
	// expand control and elims to table
	int exp_rat, new_quick_rat,
		exp_lim, exp_lim_e, 
		nexp_telims,nexp_gotelims,ntxyelim;
	CAND exp_telims[50],exp_gotelims[50];


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
	int IsTarget(int d, int c) {
		if (all_target_cells.Off_c(c)) return 0;
		for (int i = index_all_targets; i < xyew.netbivcands; i++) {
			CAND cd = xyew.etbivcands[i].GetB();
			if (cd.Digit() == d && cd.Cell() == c) 	return 1;			
		}
		return 0;
	}
	int IsTarget(int d,BF128 toff) {
		dtarget = d;
		for (int i = index_all_targets; i < xyew.netbivcands; i++) {
			CAND cd = xyew.etbivcands[i].GetB();
			int dd = cd.Digit();
			ctarget = cd.Cell();
			if (dd != d)continue;
			if (toff.On_c(ctarget)) {
				CAND cd2 = xyew.etbivcands[i].GetA();
				end_d1= cd2.Digit();end_c1= cd2.Cell();
				return 1;	
			}
		}
		return 0;
	}	
	inline void PrintElim(int ispot);
	int Init();
	int Er71();
	void Er71B(int cpt);// after a valid perm for cells 

	int Er7x();// after all perms done
	int Er7xmode(int mode);//  guven search case
	int Er7xmodeGo();//  guven search case

	int Er7xStarts();// find all starts keep if >=2
	int Er7aw(); // all starts  
	void DoElims(int ispot);
	void NewSol(char* zs,int cpt=0) {
		BF128 wor[9]; memset(wor,0,sizeof wor);
		for (int i = 0; i < 81; i++) {
			int d = zs[i];		if (d > 8)continue;
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
		for (int i = 0; i < 9; i++) {
			if (andf[i].isNotEmpty()) {
				aig = 1;
			}
		}		
		return  aig;
	}
	int GetTcand(BF128 * o,CAND* t) {
		register int n = 0, c;
		CAND cd;
		for (int i = 0; i < 9; i++) {
			BF128 w = o[i]; if (w.isEmpty())continue;
			while ((c = w.getFirstCell())>=0) {
				w.Clear_c(c);
				cd.Set(i, c); 
				if(n<50)t[n++] = cd;

			}
		}
		return n;
	}

	void StatusBiv() {
		cout << "cset dig nodes status" << endl;
		NameBf128List("csets", csets);
		NameBf128List("dig_m", dig_m);
		char ld[5] = { 'd',' ',' ',0 };
		for (int i = 0; i < 9; i++) if (cset_d[i].isNotEmpty()) {
			ld[2] = i + '1'; NameBf128List(ld, cset_d[i]);
		}
		cout << "dig nodes" << endl;
		for (int i = 0; i < 9; i++) if (dig_nodes[i].isNotEmpty()) {
			ld[2] = i + '1'; NameBf128List(ld, dig_nodes[i]);

		}
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

	if(debug)StatusBiv();
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
	return Er71();
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
			if (sw == cspot) return Er7x(); // closed
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
	CSPOT& s = cspot[ncsets];
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
//int xybivnst[5] = { 3,4,6,8, };//n pairs (ispot) 71 72 73 74 then 75

//============== after common 71 find aliminations
inline void XYBIV::PrintElim(int ispot) {
#ifdef SEROUT
	xyew.Status();
	cout << serate.er << " target reached or exit ispot =" << ispot << " "
		<< celim_d + 1 << cell_names[celim_c] << "   ";
	for (int i = 0; i < ispot; i++) {
		XYS& s = xys[i];
		cout << "~" << s.cand1.Out(cout1) << "+" << s.cand2.Out(cout2) << "  ";
	}
	cout << "~" << dtarget + 1 << cell_names[ctarget];
	cout << "+" << end_d1 + 1 << cell_names[end_c1]
		<< " nexp_gotelims=" << nexp_gotelims << endl;
#endif
}

int XYBIV::Er7xStarts() {
	if (!Elims()) return 0;// elims in andf[9]
	nexp_telims = GetTcand(andf, exp_telims);
	ntxyelim = 0;
	for (ielim = 0; ielim < nexp_telims; ielim++) {// try each elim
		memset(&xyew, 0, sizeof xyew);// working area for new cand
		xyew.Init(exp_telims[ielim]);// enter cand,dig,cell
		register int d = xyew.ce_d, c = xyew.ce_c;
		BF128  wd = dig_nodes[d] | cset_d[d];
		if (csets.On_c(c))xyew.stopifelim = 1;
		if (dig_nodes[d].On_c(c))xyew.stopifelim = 1;
		BF128 wda = wd & cell_z3x[c], wda2 = wda;
		if (wd.On_c(c))wda2.Set_c(c);
		int v = sv.cells[c] ^ (1 << d), dd;// other digits
		while (v) {// start dig biv in cell
			bitscanforward(dd, v); v ^= (1 << dd);
			if (dig_nodes[dd].On_c(c)) {// a new dig in cell 
				xyew.Add0(dd);
				if (10) {
					int c2;
					BF128 w = dig_nodes[dd] & cell_z3x[c];// usually one cell
					while ((c2 = w.getFirstCell()) >= 0) {
						w.Clear_c(c2);
						if (IsNotBiv(dd, c, c2)) continue;
						xyew.etbivcands[xyew.netbivcands++].Set(dd, c, dd, c2);
					}
				}
			}
		}
		xyew.Add(dig_nodes[d] & cell_z3x[c], cset_d[d] & cell_z3x[c]);
		//Add digit bi values
		{
			int c1, c2;
			BF128 x = dig_nodes[d] & cell_z3x[c];
			while ((c1 = x.getFirstCell()) >= 0) {
				x.Clear_c(c1);
				BF128 w2 = dig_nodes[d] & cell_z3x[c1];// usually one cell
				w2.Clear_c(c);// no redundancy
				while ((c2 = w2.getFirstCell()) >= 0) {
					w2.Clear_c(c2);
					if (IsNotBiv(d, c1, c2)) continue;
					xyew.etbivcands[xyew.netbivcands++].Set(d, c1, d, c2);
				}
			}
		}
		// Add cells bi values 
		{
			int c1, d1;
			BF128 x = cset_d[d] & cell_z3x[c];
			while ((c1 = x.getFirstCell()) >= 0) {
				x.Clear_c(c1);
				int v = sv.cells[c1] ^ (1 << d);
				if (!v)continue;// safety against redundancy
				bitscanforward(d1, v);
				xyew.etbivcands[xyew.netbivcands++].Set(d, c1, d1, c1);
			}
		}
		if (xyew.ntot < 2)continue;
		txyelim[ntxyelim++] = xyew;

	}
#ifdef SEROUT
	cout << " seen starts " << ntxyelim  << endl;
	//if( solve.step==7)
	//for (int i = 0; i < ntxyelim; i++)txyelim[i].Status();

#endif

	return ntxyelim;
}
int XYBIV::Er7aw() {// all starts try a pair
	exp_lim_e = exp_lim;// start with reached limit
	celim_c = xyew.ce_c; celim_d = xyew.ce_d;
	cand_elim.Set(celim_d , celim_c);
	myelimdone = 0;
	for (int i1 = 0; i1 < xyew.netbivcands-1; i1++) {
		bc1 = xyew.etbivcands[i1];
		bc1.Get(st_d1, st_c1, st_d2, st_c2);
		all_target_cells.SetAll_0();
		index_all_targets = i1 + 1;
		int n = 0;
		for (int i2 = index_all_targets; i2 < xyew.netbivcands; i2++) {
			bc2 = xyew.etbivcands[i2];
			bc2.Get(end_d1, end_c1, end_d2, end_c2);
			if (end_d1 == st_d1 && end_c1 == st_c1) continue;
			all_target_cells.Set_c(end_c2);
			n++;
		}
		{
			edebug = 0;
			XYS* s = xys;
			memset(xys, 0, sizeof xys[0]);
			s->cand1.Set(st_d1, st_c1);		s->cand2.Set(st_d2, st_c2);
			s->cdpm.Set(celim_d, celim_c);
			(++s)->DoStep();
			if (myelimdone) return 1;
		}
	}
	return 0;
}
int XYBIV::Er7x() {
	if (!Er7xStarts()|| (!ntxyelim) )return 0;
#ifdef SEROUT
	cout << "entry er7x rating  still there " << ntxyelim << endl;
#endif
	if (serate.er < 75) {
		new_quick_rat = 71; 
		if (serate.er > new_quick_rat)new_quick_rat = serate.er;
		while (new_quick_rat < 75) {
			if(Er7xmode(new_quick_rat)) return 1;
			new_quick_rat++;
		}
		if (Er7xmode(0)) return 1;
		return Er7xmode(7552);
	}
	new_quick_rat = 73; // rating is already 75 
	if (Er7xmode(0)) return 1;
	return Er7xmode(7552);
	if (Er7xmode(new_quick_rat)) return 1;

}
int XYBIV::Er7xmode(int mode) {
	//int nst[5] = { 2,4,6,8,20 };//n pairs (ispot) 71 72 73 74 then 75
	//int erispot[7] = { 71,72,72,73,73,74,74 };// rating on ispot-2
	switch (mode) {
	case 71: {	exp_rat = 71; exp_lim = 2;	return Er7xmodeGo();	}
	case 72: {	exp_rat = 72; exp_lim = 3;	return Er7xmodeGo();	}
	case 73: { exp_rat = 73; exp_lim = 6;	return Er7xmodeGo(); }
	case 74: { exp_rat = 74; exp_lim = 8;	return Er7xmodeGo(); }
	case 753: {
		exp_rat = 73; exp_lim = 6;
		return Er7xmodeGo();
	}
	case 7552: {
		exp_rat = 75; exp_lim = 20;
		return Er7xmodeGo();
	}
	default: {
		exp_rat = 75; exp_lim = 9;
		return Er7xmodeGo();
	}
	}// end switch
}
int XYBIV::Er7xmodeGo() {
	ielim_last = -1;//not a valid
	nexp_gotelims = elimdone = endelimdone = iret = 0;
	//cout << ntxyelim << " elims to try from store" << endl;
	for (int ielim2 = 0; ielim2 < ntxyelim; ielim2++) {// try each elim stored
		xyew = txyelim[ielim2];
		if (!xyew.stopifelim) continue;		if (Er7aw()) 	return 1;
	}
	for (int ielim2 = 0; ielim2 < ntxyelim; ielim2++) {// try each elim stored
		xyew = txyelim[ielim2];
		if (xyew.stopifelim) continue;	Er7aw();
	}
	//cout << " end 7x elidone= " << elimdone << endl;
	if (elimdone) return 1;
	if (nexp_gotelims) {
#ifdef SEROUT
		cout << " elim from store " << nexp_gotelims << endl;
#endif
		for (int i = 0; i < nexp_gotelims; i++) {// try each elim
			CAND cd = exp_gotelims[i];
			p->Clear(cd.Digit(), cd.Cell());
		}
		serate.SetRating(exp_rat);		return 1;
	}
	return 0;
}




void XYBIV::DoElims(int ispot) {
	PrintElim(ispot);	if (myelimdone) return;
	exp_lim_e=ispot-1;// only lower chains for same elim
	//int nst[5] = { 3,4,6,8,20 };//n pairs (ispot) 71 72 73 74 then 75
	int nst[5] = { 2,4,6,8,20 };//n pairs (ispot) 71 72 73 74 then 75
	int erispot[7] = { 71,72,72,73,73,74,74 };// rating on ispot-2

	int rating = 75;	if (ispot < 3)rating = 71;
	else if (ispot <= 8)rating = erispot[ispot-2];
	int lim = nst[rating - 71];

	if (rating == new_quick_rat) { exp_lim = lim; serate.SetRating(new_quick_rat); }

	if (rating <= serate.er) {// immediate clearing
		//cout << "immediate clearing" << endl;
		iret = 1; elimdone++; myelimdone++;
		p->Clear(celim_d, celim_c);
		return;
	}
	if (rating < exp_rat) {
		//cout << "lower xy rating " << rating << endl;
		exp_rat = rating;
		if (rating >= 71)		exp_lim = nst[rating - 71];
		else exp_lim = 2;// safety
		nexp_gotelims=0;	ielim_last = -1;//not a valid

	}
	if (exp_lim > 6) { exp_lim = exp_lim_e = 6; }// only one 75 searched
	if (ielim != ielim_last) {
		exp_gotelims[nexp_gotelims++] = cand_elim;
		ielim_last = ielim;
	}
}



//==============  AICs search
void XYBIV::XYS::DoStep() {
	if (xybiv.myelimdone)return;//finished
	int debug = 0;
	SOLV81& p = xybiv.sv;
	Init(*(this - 1));
	if (ispot > xybiv.exp_lim_e) return;// safety
	int d = cand2.Digit(), c = cand2.Cell(), cx = cand1.Cell();
	used_cells.Set_c(c); used_cells.Set_c(cx);
	// stop if target reached on or loop
	if (c == xybiv.st_c1 && d == xybiv.st_d1) return;
	if (xybiv.IsTarget(d, c))  return;	

	if (debug > 1) {
		cout << "xy do step start ispot=" << ispot << " "
			<< cand2.Out(cout1) ;	NameBf128List(" uc ", used_cells);
	}
	BF128 wd = (p.dm[d] & cell_z3x[c])-used_cells,
		wdc = wd & p.ccm[1],
		wdd = wd & xybiv.dig_nodes[d],
		toff= (wdc | wdd)& xybiv.all_target_cells;
	//toff.Clear_c(cx); // no way back
	// look for target reached
	if (toff.isNotEmpty()) {
		if (debug) {
			cout << ispot;	NameBf128List("target off expected", toff);
			NameBf128List("used cells", used_cells);
		}
		if (xybiv.IsTarget(d,toff)) {
			xybiv.DoElims(ispot);	return;	}
	}
	if (ispot >= xybiv.exp_lim_e) return;
	if (xybiv.myelimdone) return;
	if (DoNewDC(d, c)) return;// next digit bivalue starts  same cell
	if (DoNewD(d, wdd)) return;// next possible bivalue same digit
	if (DoNewC(d, wdc)) return;// next possible bi value cell
}
//____________ new biv is a digit biv start in last cell
inline int  XYBIV::XYS::DoNewDC(int d,int c) {
	if (xybiv.dig_m.Off_c(c)) return 0;
	{// other digit biv same cell
		int v = xybiv.sv.cells[c], d2; v ^= 1 << d;
		//if (debug) cout<<Char9out(v) << " v go new dc" << endl;
		while (v) {
			bitscanforward(d2, v);	v ^= 1 << d2;
			if (xybiv.dig_nodes[d2].Off_c(c)) continue;

			DoNewDcom(d2, c);// Find the bi value
			if (xybiv.myelimdone) return 1;
		}
	}	
	return xybiv.myelimdone;
}
//__________ new biv is a digit bi value
inline int  XYBIV::XYS::DoNewD(int d, BF128 w) {
	if (w.isEmpty()) return 0;
	//cout << " DoNewD " << d + 1 ;
	//NameBf128List(" for cells ", w);
	SOLV81& p = solve.sv81w;
	int c;
	while ((c = w.getFirstCell()) >= 0) {
		w.Clear_c(c); 
		DoNewDcom(d, c);// Find the bi value
		if (xybiv.myelimdone) return 1;
	}
	return xybiv.myelimdone;
}
//__________ new digit  biv after first cell da ca 
inline int  XYBIV::XYS::DoNewDcom(int da,int ca) {
	SOLV81& p = xybiv.sv;
	int c;
	BF128 w = ((p.dm[da] & xybiv.dig_nodes[da]) & cell_z3x[ca])-used_cells;
	if (ispot >= xybiv.exp_lim) return xybiv.myelimdone;
	while ((c = w.getFirstCell()) >= 0) {
		w.Clear_c(c);
		cand1.Set(da, ca); cand2.Set(da, c);
		if (cdpm.On(cand1) || cdpm.On(cand2)) continue;
		if (xybiv.IsNotBiv(da, ca, c)) continue;
		(this + 1)->DoStep();
		if (xybiv.myelimdone) return 1;
	}
	return xybiv.myelimdone;
}
//__________ new  cell BI VALUE
inline int XYBIV::XYS::DoNewC(int d,BF128 w) {
	if (w.isEmpty()) 	return 0;
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
		if (ispot >= xybiv.exp_lim)	return xybiv.myelimdone;

		if (cdpm.On(cand1) || cdpm.On(cand2)) continue;
		(this + 1)->DoStep();
		if (xybiv.myelimdone) return 1;
	}
	return xybiv.myelimdone;
}
