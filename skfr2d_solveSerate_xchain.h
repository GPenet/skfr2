
struct ER65S {
	struct XCH {// for one step expand
		BF128  txk;// cum killed
		int ispot, ck, ca,target,lim;// cells
		void Initcy(int eca, int ecb, BF128 ecebf, int elim);
		void DoStepCy();
		void Initch(int eca, int ecb, BF128 ecebf, int elim);
		void DoStepCh();
	}xch[10];
	BF128 elims,// known elims from global search
		zeab,// elims for a chain search
		df,//digit field unsolved
		dfb,// field of cells belonging to bi values
		dmu[27]; // field per unit
	int u_uns,//unsolved units
		ubiv,// unit with a bi value (bit field)
		digit,rating,nlim,step,iret;
	//________ cycle and chain start/end search
	void Cy_search(int ca, int cb,  BF128 cebf, int n);
	void CyDoElims(int ispot);
	void Ch_search(int ca, int cb, BF128 cebf, int n);
	void ChDoElims(int ispot);
	BF128 Get_cell_biv(int cell) {
		BF128 wr; wr.SetAll_0();
		int cb = tcellsrcb[cell] & ser65.ubiv, iu;
		//cout << Char27out(cb)
		//	<< " get biv s) " << cell_names[cell] << endl;
		BF128 w = (dfb & cell_z3x[cell]) ;
		while (cb) {
			bitscanforward(iu, cb);
			cb ^= 1 << iu;
			wr |= (w & units3xBM[iu]);
		}
		return wr;
	}
	BF128 Get_all_biv(BF128 cbf) {
		BF128 wr; wr.SetAll_0();
		int cell;
		while ((cell=cbf.getFirstCell())>=0) {
			cbf.Clear_c(cell);
			wr |= Get_cell_biv(cell);
		}
		return wr;
	}
	//===== Nishio process
	struct NS {// spot nishio
		BF128 pm,killed;
		int ispot,u_in,c_in,uns_us, u0s, u1s;
		void InitStart(int c);
		void Init(NS& o) {
			*this = o;	ispot++;
		}
		void DoNishioStep(int c,int u);
	}spnish[10];// can not exceed 9 assign
	struct NSOK {// pending list of elims same rating
		int units1,  units2;
		int digit,cell,  count1, count2,count_t,drating;
		void GetPath1(); void GetPath2();
		void Status() {
			cout << "status nsok for digit " << digit + 1<< cell_names[cell]
				<< " count1 " << count1 << " count2 " << count2 
				<<" index drating " <<drating << endl;

		}
	}nsok[50],nsokw;
	int nmax_sp,n_nsok,dif_rating,d_dif,c_dif,er_dif,elimdone;
	BF128 dtokill, dkilldone;
	int dynstep,rdynstep,dynunits,dynunits2;
	void DynKill();
	void Path(const char* lib);
	void NishioCont(int ispot);
	void AddElim();
}ser65;
//====================== cycle search
void ER65S::Cy_search(int ca, int cb, BF128 cebf, int n) {
	nlim = n;
	xch[0].Initcy(ca, cb, cebf, n);// ca off start
}
void ER65S::XCH::Initcy(int eca, int ecb, BF128 cebf, int n) {
	memset(this, 0, sizeof xch[0]);
	// first ca off -> cb on
	target = ecb; lim = n;
	txk.Set_c(eca); ck = eca;
	txk |= cebf;// be sure not to use elim
	// can be several biv as start
	int cb = tcellsrcb[ck] & ser65.ubiv, iu;
	BF128 w = (ser65.dfb & cell_z3x[ck]) - txk;
	while (cb) {
		bitscanforward(iu, cb);
		cb ^= 1 << iu;
		BF128 wu = w & units3xBM[iu];
		if (wu.isEmpty()) continue;// safety
		ca = wu.getFirstCell();
		//cout << "go x docy" << cell_names[ck] << " " << cell_names[ca] << endl;
		(this + 1)->DoStepCy();
	}
}
void ER65S::CyDoElims(int ispot) {
	iret = 1;
	solve.sv81w.Clean(digit, zeab);
	NameBf128List(" clean zeab", zeab);
	for (int i = 0; i < ispot; i++) {
		XCH& s = xch[i],sn= xch[i+1];
		BF128 e = (elims & cell_z3x[sn.ck]) & cell_z3x[s.ca];
		if (e.isNotEmpty()) {
			solve.sv81w.Clean(digit, e);
			NameBf128List(" clean e ", e);

		}
	}

#ifdef SEROUT
	cout << " valid x cycle digit " << digit + 1;
	NameBf128List(" elims chain ", zeab);
	for (int i = 0; i <= nlim; i++) {
		XCH& s = xch[i];
		cout << "~" << cell_names[s.ck] << " " << cell_names[s.ca] << " ";
		cout << endl;
	}
#endif		
}
//_________cycle  recursive loop n steps max 
void ER65S::XCH::DoStepCy() {
	if (ser65.iret)return;//finished 
	int debug = 0;
	XCH& o = *(this - 1);	*this = o;	ispot++;
	BF128 xk = ser65.dfb & cell_z3x[o.ca];
	xk -= txk; txk |= xk;
	if (debug > 1)NameBf128List(" xk in step ", xk);
	// find new assigned in bi values
	while ((ck = xk.getFirstCell()) >= 0) {
		xk.Clear_c(ck);
		int cb = tcellsrcb[ck] & ser65.ubiv, iu;
		if (debug)cout << Char27out(cb)
			<< " get ubiv  " << cell_names[ck] << " sp " << ispot
			<< " lim " << lim << endl;
		BF128 w = (ser65.dfb & cell_z3x[ck]) - txk;
		w.Clear_c(o.ca);
		if (debug > 1)NameBf128List(" w ", w);
		while (cb) {
			bitscanforward(iu, cb);
			cb ^= 1 << iu;
			BF128 wu = w & units3xBM[iu];
			if (wu.isEmpty()) continue;// safety
			ca = wu.getFirstCell();
			if (debug > 1)NameBf128List(" wu ", wu);
			if (ca != target) {
				if (ispot >= lim)continue;
				(this + 1)->DoStepCy();
				continue;
			}
			ser65.CyDoElims(ispot);
			return;
		}
	}
}

//====================== chain search 
void ER65S::Ch_search(int ca, int cb, BF128 cebf, int n) {
	nlim = n;
	xch[0].Initch(ca, cb, cebf, n);// ca off start
}
void ER65S::XCH::Initch(int eca, int ecb, BF128 cebf, int n) {
	memset(this, 0, sizeof xch[0]);
	// first ca off -> cb on
	target = ecb; lim = n;
	txk.Set_c(eca); ck = eca;
	txk|=cebf;// be sure not to use elim
	// can be several biv as start
	int cb = tcellsrcb[ck] & ser65.ubiv, iu;
	//cout << Char27out(cb)
	//	<< " get biv s) " << cell_names[ck] << endl;
	BF128 w = (ser65.dfb & cell_z3x[ck]) - txk;
	while (cb) {
		bitscanforward(iu, cb);
		cb ^= 1 << iu;
		BF128 wu = w & units3xBM[iu];
		if (wu.isEmpty()) continue;// safety
		ca = wu.getFirstCell();
		//cout << "go doch" << cell_names[ck] << " " << cell_names[ca] << endl;
		(this + 1)->DoStepCh();
	}
}
void ER65S::ChDoElims(int ispot) {
	iret = 1;
	solve.sv81w.Clean(digit, zeab);
	elims -= zeab;
#ifdef SEROUT
	cout << " valid x chain digit " << digit + 1;
	NameBf128List(" elims chain ", zeab);
	for (int i = 0; i <= ispot; i++) {
		XCH& s = xch[i];
		cout << "~" << cell_names[s.ck] << " " << cell_names[s.ca] << " ";
		cout << endl;
	}
#endif		
}
//_________ chain recursive loop n steps max 
void ER65S::XCH::DoStepCh() {
	if (ser65.iret)return;//finished 
	int debug = 0;
	XCH& o = *(this - 1);	*this = o;	ispot++;
	BF128 xk = ser65.dfb & cell_z3x[o.ca];
	xk -= txk; txk |= xk;
	if (debug>1)NameBf128List(" xk in step ", xk);
	// find new assigned in bi values
	while ((ck = xk.getFirstCell()) >= 0) {
		xk.Clear_c(ck);
		int cb = tcellsrcb[ck] & ser65.ubiv, iu;
		if (debug )cout << Char27out(cb)
			<< " get ubiv  " << cell_names[ck]<< " sp " << ispot 
			<<" lim "<<lim << endl;
		BF128 w = (ser65.dfb & cell_z3x[ck]) - txk;
		w.Clear_c(o.ca);
		if (debug>1)NameBf128List(" w ", w);
		while (cb) {
			bitscanforward(iu, cb);
			cb ^= 1 << iu;
			BF128 wu = w & units3xBM[iu];
			if (wu.isEmpty()) continue;// safety
			ca = wu.getFirstCell();
			if (debug>1)NameBf128List(" wu ", wu);
			if (ca != target) {
				if (ispot >= lim)continue;
				(this + 1)->DoStepCh();
				continue;
			}
			ser65.ChDoElims(ispot);
		}
	}
}

// =====================  start search all digits level xx
#ifdef SEROUT
#endif

int SOLV81::Er6x() {// look for X chains
	int debug = 0;
	int iret = 0;
	//work on an elim start not in unit
	if(debug)cout << "er6x rating " << ser65.rating << endl;
	int ce, ca, cb;// cell elim to try
	BF128 x = ser65.elims, rx = x;
	while ((ce = x.getFirstCell()) >= 0) {
		x.Clear_c(ce);
		//cout << "ce " << cell_names[ce] << endl;
		BF128 sts = ser65.dfb & cell_z3x[ce];
		while ((ca = sts.getFirstCell()) >= 0) {
			sts.Clear_c(ca);
			int uas = tcellsrcb[ca];
			BF128 y = sts,
				zea = ser65.df & cell_z3x[ca];
			while ((cb = y.getFirstCell()) >= 0) {
				y.Clear_c(cb);
				ser65.zeab = zea & cell_z3x[cb];
				if ((ser65.zeab & rx) != ser65.zeab) 	continue;// not a cycle elim				
				int ubs = tcellsrcb[cb];
				if (uas & ubs) {
					if (debug) {
						cout << " possible start Xcycle "
							<< cell_names[ca] << " " << cell_names[cb];
						NameBf128List(" cell elims ", ser65.zeab);
					}
					int nst[4] = { 2, 3, 4, 9 }, ns = nst[ser65.rating - 65];
					ser65.iret = 0;
					ser65.Cy_search(ca, cb, ser65.zeab, ns);
					// stop at first
					if (ser65.iret) {						
						if (debug)cout << "seen x cycle ok" << endl;
						return 1;
					}
				}
			}
		}
	}
	if (ser65.rating == 65) return 0;
	// no active cycle try chain
	//NameBf128List(" all elims ", x);
nextelims: {
	x = ser65.elims;
	while ((ce = x.getFirstCell()) >= 0) {
		x.Clear_c(ce);
		BF128 sts = ser65.dfb & cell_z3x[ce];
		while ((ca = sts.getFirstCell()) >= 0) {
			sts.Clear_c(ca);
			int uas = tcellsrcb[ca];
			BF128 y = sts,
				zea=ser65.df & cell_z3x[ca];
			while ((cb = y.getFirstCell()) >= 0) {
				y.Clear_c(cb);
				int ubs = tcellsrcb[cb];
				if (!(uas & ubs)) {
					ser65.zeab = zea & cell_z3x[cb];
					if ((ser65.zeab & rx) != ser65.zeab) continue;// not a chain elim					
					int nst[4] = { 1,2,3,5 }, ns = nst[ser65.rating - 66];
					if (debug) {
						cout<<ns << " possible start Xcchain "
							<< cell_names[ca] << " " << cell_names[cb];
						NameBf128List(" cell elims " , ser65.zeab);
					}
					ser65.iret = 0;
					ser65.Ch_search(ca, cb, ser65.zeab, ns);
					if(ser65.iret){
						if (debug)cout << "seen x chain ok " << cell_names[ca] << " " << cell_names[cb] << endl;
						// stop if new assign expected
						if ((ser65.dfb& ser65.zeab).isNotEmpty())	return 1;
						iret++;
						goto nextelims;
					}
				}
			}
		}
	}

	}
	//cout << "end chains iret= " << iret << endl;
	return iret;
}
int SOLV81::DoEr6xD(int d, int r) {
	ser65.elims = solve.rclean1[d];
	if (ser65.elims.isEmpty()) return 0;
	ser65.digit = d;	ser65.rating = r;
#ifdef SEROUT
	if (SEROUT == 65) {
		ImageOne(d);
		NameBf128List("Elims list:", ser65.elims);
	}
#endif
	ser65.df = dm[d] & unsolved_cells;
	ser65.dfb.SetAll_0();
	ser65.ubiv=ser65.u_uns = 0;
	for (int iu = 0, bit = 1; iu < 27; iu++, bit <<= 1) {
		BF128 wu = ser65.df & units3xBM[iu];
		ser65.dmu[iu] = wu;
		if (wu.isNotEmpty()) {
			ser65.u_uns |= bit;
			if (wu.Count96() == 2) { ser65.dfb |= wu; ser65.ubiv |= bit; }
		}
	}
	return Er6x();
}
int SOLV81::DoEr6x(int rat) {
	if (rat < 65 || rat>69) return 0; //safety, current limit
	int iret = 0;
	for (int i = 0; i < 9; i++)
		if (serate.activedigits & (1 << i)) {
			iret += DoEr6xD(i, rat);
		}
	if (iret) { serate.SetRating(rat); return 1; }
	return iret;
}

int SOLV81::DoEr75X() {//Nishio 


	ser65.nmax_sp = 9;//max number of assigned expected 
	ser65.n_nsok = 0; 
	ser65.dif_rating = 30;
	ser65.er_dif = serate.er - 75;
	int iret = 0,cell;
	for (int d = 0; d < 9; d++)
		if (serate.activedigits & (1 << d)) {
			ser65.elims = solve.rclean1[d];
			if (ser65.elims.isEmpty()) continue;;
#ifdef SEROUT
			if (SEROUT == 75) {
				cout << "entry new DoEr75 Nishio()  " << endl;
				ImageOne(d);
				NameBf128List("Elims list:", ser65.elims);
			}
#endif
			ser65.digit = ser65.nsokw.digit = d;
			ser65.df = dm[d] & unsolved_cells;
			ser65.u_uns=0;
			for (int i = 0, bit = 1; i < 27; i++, bit <<= 1) {
				if( (ser65.df & units3xBM[i]).isNotEmpty())
					ser65.u_uns |= bit;
			}
			//cout << Char27out(ser65.u_uns) << " unsolved units" << endl;
			//ImageOne(d);NameBf128List(" Elims list:", ser65.elims);
			BF128 x = ser65.elims;
			while ((cell = x.getFirstCell()) >= 0) {
				x.Clear_c(cell);
				ser65.nsokw.cell = cell;
				ser65.elimdone = 0;
				ser65.spnish[0].InitStart(cell);
			}
		}
	if (ser65.n_nsok) {
		int r = 75 + ser65.dif_rating;
#ifdef SEROUT
		cout << "Nishio active n=" << ser65.n_nsok
			<< " for rating " << r << endl;
#endif
		// can be redundant eliminations
		for (int i = 0; i < ser65.n_nsok; i++) {
			ER65S::NSOK& ns = ser65.nsok[i];
			if(solve.sv81w.dm[ns.digit].On_c(ns.cell))
				solve.sv81w.Clear(ns.digit, ns.cell);
		}
		serate.SetRating(r); 
		return 1;
	}

	return 0;
}



/* strategy for Nishio BF128 pm,killed, hit;
 stop if contradiction reached in "n" steps
 one step -> clean seen/ get new singles and contradictions in sets */
void ER65S::NS::InitStart(int c) {
	memset(this, 0, sizeof spnish[0]);
	c_in = c; u_in =-1;
	uns_us = ser65.u_uns;
	pm = ser65.df;
	//cout << "  try " << cell_names[c] << endl;
	(this + 1)->DoNishioStep(c, -1);
}

void ER65S::NS::DoNishioStep(int c,int uin) {// new cell on 
	if (ser65.elimdone) return;
	Init(*(this - 1));
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
		ser65.NishioCont(ispot);
		return;
	}
	if (ispot >= ser65.nmax_sp) return;
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
void ER65S::DynKill() {
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
void ER65S::NSOK::GetPath1() {
	units1 = ser65.dynunits;
	count1 = ser65.dkilldone.Count96();
}
void ER65S::NSOK::GetPath2() {
	units2 = ser65.dynunits;
	count2 = ser65.dkilldone.Count96() + 1;//  + last on
	count_t = count1 + count2;
	drating = GetDifficultyIndex(count_t);
}
void ER65S::NishioCont(int ispot) {
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
void ER65S::AddElim() {
	int aig = 1, dd = nsokw.digit, cc = nsokw.cell;
	if (d_dif == dd && c_dif == cc) return;
	for (int i = 0; i < n_nsok; i++) {
		NSOK& ns = nsok[i];
		if (dd == ns.digit && cc == ns.cell) return;
	}
	//nsokw.Status();
	nsok[n_nsok++] = nsokw;
}
void ER65S::Path(const char* lib) {
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

