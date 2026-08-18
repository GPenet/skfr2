
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
	int ubiv,// unit with a bi value (bit field)
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
	for (int i = 0; i < ispot; i++) {
		XCH& s = xch[i],sn= xch[i+1];
		BF128 e = (elims & cell_z3x[sn.ck]) & cell_z3x[s.ca];
		if (e.isNotEmpty()) {
			solve.sv81w.Clean(digit, e);

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
	XCH& o = *(this - 1);	*this = o;	ispot++;
	BF128 xk = ser65.dfb & cell_z3x[o.ca];
	xk -= txk; txk |= xk;
	// find new assigned in bi values
	while ((ck = xk.getFirstCell()) >= 0) {
		xk.Clear_c(ck);
		int cb = tcellsrcb[ck] & ser65.ubiv, iu;
		BF128 w = (ser65.dfb & cell_z3x[ck]) - txk;
		w.Clear_c(o.ca);
		while (cb) {
			bitscanforward(iu, cb);
			cb ^= 1 << iu;
			BF128 wu = w & units3xBM[iu];
			if (wu.isEmpty()) continue;// safety
			ca = wu.getFirstCell();
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
	XCH& o = *(this - 1);	*this = o;	ispot++;
	BF128 xk = ser65.dfb & cell_z3x[o.ca];
	xk -= txk; txk |= xk;
	// find new assigned in bi values
	while ((ck = xk.getFirstCell()) >= 0) {
		xk.Clear_c(ck);
		int cb = tcellsrcb[ck] & ser65.ubiv, iu;
		BF128 w = (ser65.dfb & cell_z3x[ck]) - txk;
		w.Clear_c(o.ca);
		while (cb) {
			bitscanforward(iu, cb);
			cb ^= 1 << iu;
			BF128 wu = w & units3xBM[iu];
			if (wu.isEmpty()) continue;// safety
			ca = wu.getFirstCell();
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
	int iret = 0;
	//work on an elim start not in unit
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
					int nst[4] = { 2, 3, 4, 9 }, ns = nst[ser65.rating - 65];
					ser65.iret = 0;
					ser65.Cy_search(ca, cb, ser65.zeab, ns);
					// stop at first
					if (ser65.iret) 	return 1;
					
				}
			}
		}
	}
	if (ser65.rating == 65) return 0;
	// no active cycle try chain
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
					ser65.iret = 0;
					ser65.Ch_search(ca, cb, ser65.zeab, ns);
					if(ser65.iret){
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
	return iret;
}
int SOLV81::DoEr6xD(int d, int r) {
	ser65.elims = solve.rclean1[d];
	if (ser65.elims.isEmpty()) return 0;
	ser65.digit = d;	ser65.rating = r;
#ifdef SEROUT
	ImageOne(d);
	NameBf128List("Elims list:", ser65.elims);
#endif
	ser65.df = dm[d] & unsolved_cells;
	ser65.dfb.SetAll_0();
	ser65.ubiv = 0;
	for (int iu = 0, bit = 1; iu < 27; iu++, bit <<= 1) {
		BF128 wu = ser65.df & units3xBM[iu];
		ser65.dmu[iu] = wu;
		if (wu.Count96() == 2) { ser65.dfb |= wu; ser65.ubiv |= bit; }
	}
	return Er6x();
}
int SOLV81::DoEr6x(int rat) {
	if (rat < 64 || rat>69) return 0;// safety current limit
	int iret = 0;
	for (int i = 0; i < 9; i++)
		if (serate.activedigits & (1 << i)) {
			iret += DoEr6xD(i, rat);
		}
	if (iret) { serate.SetRating(rat); return 1; }
	return iret;
}


int SOLV81::DoEr75X() {//Nishio 
	cout << "entry new DoEr75 Nishio()  " << endl;
	int iret = 0;
	for (int d = 0; d < 9; d++)
		if (serate.activedigits & (1 << d)) {
			ser65.elims = solve.rclean1[d];
			if (ser65.elims.isEmpty()) continue;;
			ser65.digit = d;	
			//ImageOne(d);
			NameBf128List(" Elims list:", ser65.elims);

			//iret += DoEr6xD(i, rat);
		}
	return 0;

}