
/* dynamic expand base 85
similar to expand Nishio
expand solving last in set till contradiction
find the shortest 2 paths to get the contradiction

In fact, try first expand all 'n' steps and store if not empty
To be neutral to morphs, in the step with contradiction, 
all perms of new 'on' are tried one by one
*/
struct DYNSOK {// pending list of elims same rating
	int units1, units2;
	int digit, cell, count1, count2, count_t, drating;
	void GetPath1(); void GetPath2();
	void Status() {
		cout << "status nsok for digit " << digit + 1 << cell_names[cell]
			<< " count1 " << count1 << " count2 " << count2
			<< " index drating " << drating << endl;

	}
}dynsok[50], dynsokw;

//================= active sets
struct DYNACT {
	BF128 wcl; // last in cell
	int  uil, uild[9], uilall;// last in uint
	int  SetUp(SOLV81& p);
	inline int Isactive() {
		if (wcl.isNotEmpty())return 1;
		return  uilall;
	}
}dynact;
struct DYSN {// spot expand after N cycles
	SOLV81 sv;
	DYNACT dact;
	int ispot, cand_d,cand_cell,
		set_dc_in, c_in, uns_d[9], u0sd[9], u1sd[9];

} dys2[650], dys4[650],dysnw;
int  DYNACT::SetUp(SOLV81& p) {
	wcl = p.unsolved_cells & p.ccm[0];
	uil = 0;
	memset(uild, 0, sizeof uild);
	for (int iu = 0; iu < 27; iu++) {
		BF128 wu = p.unsolved_cells & units3xBM[iu];
		if (wu.isEmpty())continue;
		if (wu.Count96() == 1) {// last cell in unit
			uil |= 1 << iu;		continue;
		}
		for (int id = 0; id < 9; id++) {
			BF128 wud = wu & p.dm[id];
			if (wud.Count96() == 1) {//digit last in unit
				uild[id] |= 1 << iu;
			}
		}
	}
	uilall = uil;
	for (int id = 0; id < 9; id++)uilall |= uild[id];
	return Isactive();
}
struct DYS {// spot expand
	DM9 pm, killed;
	BF128 uns_c, u0sc, u1sc;
	int ispot, set_dc_in, c_in, uns_d[9], u0sd[9], u1sd[9];
	void Init(DYS& o) { *this = o;	ispot++; }
	void DoDynStep(DYS* o, CAND* tc, int ntc);
}sdys[40];
void DYS::DoDynStep(DYS* o, CAND* tc, int ntc) {// new cands on
	Init(*o);
	for (int itc = 0; itc < ntc; itc++) {// expand all
		CAND cd = tc[itc];
		int d = cd.Digit(), c = cd.Cell();

	}
}
/* return
0 if solved (not possible) or no more single
1 if waiting singles
2 if contradiction reached 

assign can give conflict new empty cell
can also be final empty digit set
*/
struct ER85S {
	int ndys2,// last in uint
		active_expand;// one contradiction active seen
	//===== dynamic process
	int nmax_sp, n_nsok, dif_rating, d_dif, c_dif, er_dif, elimdone;
	void DoFirst2Steps();

	BF128 dtokill, dkilldone;
	int dynstep, rdynstep, dynunits, dynunits2;
	void DynKill();
	void Path(const char* lib);
	void NishioCont(int ispot);
	void AddElim();
}ser85;
int SOLV81::DoER85() {
	cout << "entryDoER85() " << endl;
	if (unsolved_cells.isEmpty()) return 0;// safety
	ser85.DoFirst2Steps();
	BF128 x = unsolved_cells;
	int cx, dy, cpt = 0;
	while ((cx = x.getFirstCell()) >= 0) {
		x.Clear_c(cx);
		int  vx = cells[cx];
		vx &= ~(1 << g0[cx]); // not the valid digit
		while (vx) {
			bitscanforward(dy, vx);// next false
			vx ^= 1 << dy;
			++cpt;
			if (cpt == 1)dysnw.sv.DoER85_TE_1_2(*this, dy, cx);
		}
	}
	cout << "seen " << cpt << " false " << endl;
	return 0;
}
void ER85S::DoFirst2Steps() {
	ndys2 = active_expand=0;
}
int SOLV81::DoER85_TE_1_2(SOLV81& o, int d,int c) {
	cout << d + 1 << cell_names[c] << " DoER85_TE_1_2 " << endl;
	*this = o;
	if (Assign(d, c))return 0;// should never be
	cout << d + 1 << cell_names[c] << " DoER85_TE_1_2 " << endl;
	if (unsolved_cells.isEmpty()) return 0;// safety
	if (dynact.SetUp(*this)) return 0;
	if (DoER85LastInTE() == 2) {// cont reached
		cout<<d+1 <<cell_names[c] << "assumed bug early contradiction" << endl;
		return 0;
	}
	//============ second expand step
	if (unsolved_cells.isEmpty()) return 0;// safety
	if (dynact.SetUp(*this)) return 0;// no conflict expected here
	if (DoER85LastInTE() == 2) {// cont reached
		cout << d + 1 << cell_names[c] << "assumed bug early contradiction" << endl;
		return 0;
	}
	if (ser85.active_expand) return 0;
	if (dynact.SetUp(*this)) return 0;
	// store it for later
	return 0;
}
int  SOLV81::DoER85LastInTE() {
	BF128 w = dynact.wcl;
	register int cell,dig,ir;
	if (w.isNotEmpty()) {// singles in cell
		while ((cell = w.getFirstCell()) >= 0) {
			w.Clear_c(cell);
			int  dig, v = cells[cell];
			if (!v) return 2;// empty cell		
			bitscanforward(dig, cells[cell]);// get the dig
			if ((ir = Assign(dig, cell))) {	return 2;}
		}
	}
	if (!dynact.uilall) return 0;
	for (int iu = 0,bit=1; iu < 27; iu++,bit<<=1) {
		if (!(dynact.uilall & bit))continue;
		BF128 w = unsolved_cells & units3xBM[iu];
		if (w.isEmpty()) return 2;
		if (dynact.uil & bit) {// last in unit
			cell = w.getFirstCell();
			bitscanforward(dig, cells[cell]);// get the dig
			if ((ir = Assign(dig, cell))) { return 2; }
			continue;
		}
		for (int id = 0; id < 9; id++)if (dynact.uild[id] & bit) {
			BF128 wd = w & dm[id];
			if (wd.isEmpty()) return 2;
			cell = wd.getFirstCell();
			bitscanforward(dig, cells[cell]);// get the dig
			if ((ir = Assign(dig, cell))) { return 2; }
		}
	}
	return 0;
}

/*
c_in = c; u_in = uin;
	u1s &= ~tcellsrcb[c];// clear assigned units
	BF128 w = pm & cell_z3x[c];// killed here
	pm -= w; killed |= w;// new status
	//cout << "ispot=" << ispot << " uin " << uin + 1 << " " << cell_names[c] << endl;
	uns_us &= ~tcellsrcb[c];
	int wus = uns_us;
	for (int i = 0, bit = 1; i < 27; i++, bit <<= 1) {
		if (!(wus & bit)) continue;
		BF128 wu = pm & units3xBM[i];
		int nu = wu.Count96();
		if (nu > 1)continue;
		if (nu)u1s |= bit; else u0s |= bit;
	}
	if (u0s) {// stop on contradiction
		ser85.NishioCont(ispot);
		return;
	}
	if (ispot >= ser85.nmax_sp) return;
	// next on each new on
	int x = u1s,u;
	while (x) {
		bitscanforward(u, x);	x ^= 1 << u;
		BF128 wu = pm & units3xBM[u];
		int c = wu.getFirstCell();
		x &= ~tcellsrcb[c];// clear redundancy
		(this + 1)->DoNishioStep(c,u);
	}
}
*/

/*
int SOLV81::DoEr_5X() {//Nishio 


	ser85.nmax_sp = 9;//max number of assigned expected 
	ser85.n_nsok = 0; 
	ser85.dif_rating = 30;
	ser85.er_dif = serate.er - 75;
	int iret = 0,cell;
	for (int d = 0; d < 9; d++)
		if (serate.activedigits & (1 << d)) {
			ser85.elims = solve.rclean1[d];
			if (ser85.elims.isEmpty()) continue;;
#ifdef SEROUT
			if (SEROUT == 75) {
				cout << "entry new DoEr75 Nishio()  " << endl;
				ImageOne(d);
				NameBf128List("Elims list:", ser85.elims);
			}
#endif
			ser85.digit = ser85.nsokw.digit = d;
			ser85.df = dm[d] & unsolved_cells;
			ser85.u_uns=0;
			for (int i = 0, bit = 1; i < 27; i++, bit <<= 1) {
				if( (ser85.df & units3xBM[i]).isNotEmpty())
					ser85.u_uns |= bit;
			}
			//cout << Char27out(ser85.u_uns) << " unsolved units" << endl;
			//ImageOne(d);NameBf128List(" Elims list:", ser85.elims);
			BF128 x = ser85.elims;
			while ((cell = x.getFirstCell()) >= 0) {
				x.Clear_c(cell);
				ser85.nsokw.cell = cell;
				ser85.elimdone = 0;
				ser85.spnish[0].InitStart(cell);
			}
		}
	if (ser85.n_nsok) {
		int r = 75 + ser85.dif_rating;
#ifdef SEROUT
		cout << "Nishio active n=" << ser85.n_nsok
			<< " for rating " << r << endl;
#endif
		// can be redundant eliminations
		for (int i = 0; i < ser85.n_nsok; i++) {
			ER85S::NSOK& ns = ser85.nsok[i];
			if(solve.sv81w.dm[ns.digit].On_c(ns.cell))
				solve.sv81w.Clear(ns.digit, ns.cell);
		}
		serate.SetRating(r); 
		return 1;
	}

	return 0;
}

*/

/* strategy for dynamic  Nishio DM9 pm,killed, hit;
 stop if contradiction reached in "n" steps
 one step -> clean seen/ get new singles and contradictions in sets */
/*
void ER85S::NS::InitStart(int c) {
	memset(this, 0, sizeof spnish[0]);
	c_in = c; u_in =-1;
	uns_us = ser85.u_uns;
	pm = ser85.df;
	//cout << "  try " << cell_names[c] << endl;
	(this + 1)->DoNishioStep(c, -1);
}
*/

/*
void ER85S::DynKill() {
	dynunits2 = 0;
	for (int ist = 1; ist < dynstep; ist++) {
		NS& s = spnish[ist], & sp = spnish[ist - 1];
		BF128 ks = (s.killed - sp.killed) & dtokill;
		if (ks.isNotEmpty()) {
			dtokill -= ks; dkilldone |= ks;
			int ui = s.u_in, ci = s.c_in, bit = 1 << ui;
			//cout << ist << " ui " << ui + 1 << " " << cell_names[ci];
			//NameBf128List(" to kill ", ks);
			if (ist > 1 && (!(bit & dynunits))) {
				dynunits2 |= bit;
				dkilldone.Set_c(ci);//dummy kill of the 'on' used
			}
			if (dtokill.isEmpty()) {// closed, shorter path
				dynstep = ist; break;
			}
		}
	}
	dtokill.SetAll_0();// safety force cleaned
	// if new sets, look to clean them
	//cout << Char27out(dynunits2) << " dkill end cycle" << endl;
	if (dynunits2) {
		dynunits |= dynunits2;
		int u2;
		while (dynunits2) {
			bitscanforward(u2, dynunits2); dynunits2 ^= 1 << u2;
			dtokill |= (df & units3xBM[u2]) - dkilldone;
		}
		DynKill();
	}

}
*/
/*
void ER85S::NSOK::GetPath1() {
	units1 = ser85.dynunits;
	count1 = ser85.dkilldone.Count96();
}
void ER85S::NSOK::GetPath2() {
	units2 = ser85.dynunits;
	count2 = ser85.dkilldone.Count96() + 1;//  + last on
	count_t = count1 + count2;
	drating = GetDifficultyIndex(count_t);
}
*/
/*
void ER85S::NishioCont(int ispot) {
	nmax_sp = ispot;// new max for depth
	//cout << "seen cont depth " << ispot << endl;
	int u0s = spnish[ispot].u0s, x = u0s, u;
	while (x) {// most often one unit empty
		bitscanforward(u, x);
		BF128 wu = df & units3xBM[u];

		dynunits = 1 << u;
		x ^= dynunits;

		dynstep = rdynstep = ispot;
		dkilldone.SetAll_0();
		dtokill = wu & spnish[ispot - 1].killed;
		//NameBf128List(" dto kill call ", dtokill);
		DynKill();	nsokw.GetPath1();	//Path(" end path1 ");
		
		dynunits = 1 << u;
		dynstep = rdynstep = ispot + 1;
		dkilldone.SetAll_0();
		dtokill = wu - spnish[ispot - 1].killed;
		//NameBf128List(" dto kill call2 ", dtokill);
		DynKill();	nsokw.GetPath2();	//Path(" end path2 ");
		
		{
			register int dr = nsokw.drating;
			if (dr < er_dif) {// immediate elimination
				//cout << " immediate elim " << endl;
				elimdone = 1;
				if (dif_rating > er_dif) {
					n_nsok = 0; dif_rating = dr;
				}
				AddElim(); return;
			}
			if (dr > dif_rating) continue;
			if (dr < dif_rating) {
				//nsokw.Status();
				n_nsok = 0; dif_rating = nsokw.drating;
				d_dif = nsokw.digit; c_dif = nsokw.cell;
				nsok[n_nsok++] = nsokw;
				continue;
			}
			AddElim();// same rating
		}
	}
}
*/
/*
void ER85S::AddElim() {
	int aig = 1, dd = nsokw.digit, cc = nsokw.cell;
	if (d_dif == dd && c_dif == cc) return;
	for (int i = 0; i < n_nsok; i++) {
		NSOK& ns = nsok[i];
		if (dd == ns.digit && cc == ns.cell) return;
	}
	//nsokw.Status();
	nsok[n_nsok++] = nsokw;
}
*/
/*
void ER85S::Path(const char* lib) {
	cout << lib << " size " << dkilldone.Count96() 
		<< " stored "<<n_nsok << endl;;
	BF128 w = spnish[1].killed & dkilldone;
	if (w.isNotEmpty()) 	NameBf128List("start kill ", w);
	for (int ist = 2; ist < rdynstep; ist++) {
		NS& s = spnish[ist], & sp = spnish[ist - 1];
		BF128 ks = (s.killed - sp.killed) & dkilldone;
		int ui = s.u_in, ci = s.c_in;
		if ((1 << ui) & dynunits) {
			cout << ui + 1 << " on " << cell_names[ci];
			NameBf128List(" kill ", ks);
		}
	}

}
*/
