


struct ERYY {
	struct YCH  {// for one step expand
		BF128 tcc;//cum cells used  
		int ispot, target, digit,c1, lim;
		void Initcy(int eca, int ecb, BF128 ecebf, int d,int n);
		void DoStepCy();
		void Initch(int eca, int ecb, BF128 ecebf, int d,int n);
		void DoStepCh();

	}ych[15];	

	BF128 rclean1[9],// copy of all elims from slg
		elimsd,// known elims from global searh
		zeab,//first  elims for a search start digit 
		wdu,wbu,// in unit elims and biv
		df,//digit field unsolved
		dfb,// field of cells belonging to bi values
		dmu[27]; // field per unit
	int ubiv,// unit with a bi value (bit field)
		digit,unit, ca,cb, rating, // start
		nlim, step,iret;
	void Cy_search(int ca, int cb, int n);
	void CyDoElims(int ispot);
	void Ch_search(int ca, int cb, int n);
	void ChDoElims(int ispot);
}seryy;


//================================= y cycle search

// must have elims digit/ unit 
int SOLV81::DoEr6ycy(int rat) {
	if (rat < 66 || rat>70) return 0;// safety
	int debug = 0 ;
	int nst[5] = { 3,5,7,9,20 },	ns = nst[rat - 66];
	if (debug)cout << "DoEr6ycy " << rat << " ns "<<ns << endl;
	seryy.rating = rat;	seryy.iret = 0;
	memcpy(seryy.rclean1, slgybiv.rclean1, 9 * sizeof seryy.rclean1[0]);
	// rclean can have plenty ofg derived eliminations
	for (int id = 0; id < 9; id++) {
		BF128 wde = seryy.rclean1[id];
		if (wde.isEmpty()) continue;
		if (debug) { cout << " for digit " << id + 1;		NameBf128List(" elims ", wde); }
		seryy.digit = id; seryy.elimsd = wde;
		for (int iu = 0; iu < 27; iu++) {
			BF128 wdeu=wde & units3xBM[iu];
			if (wdeu.isEmpty()) continue;
			BF128 df2a = (dm[id] & units3xBM[iu]) & ccm[1],df2;
			df2a -= wdeu; // no elim in the loop
			int nf2a = df2a.Count96();
			if (nf2a < 2) continue;
			if (nf2a == 2)df2 = df2a; 
			else {// to see >2 with canibalisation elims
				df2=df2a-wdeu;
				if (df2.Count96() != 2)continue;// to see later small windows for more
			}
			int ca = df2.getFirstCell(), cb = df2.getLastCell();
			if (debug) {
				cout << "pot y cycle start d=" << id + 1 << " cells "
					<< cell_names[ca] << " " << cell_names[cb];
				NameBf128List(" for elims ", wdeu);	
			}
			seryy.zeab = wdeu;			seryy.wbu = df2;
			seryy.iret = 0;
			seryy.Cy_search(ca, cb, ns);
			// stop at first
			if (seryy.iret){
				if (debug)cout << "seen ycycle ok" << endl;
				serate.SetRating(rat);
				return 1;
			}
		}
	}
	return 0;
}

void ERYY::Cy_search(int eca, int ecb, int n) {
	ca = eca; cb = ecb;
	int debug = 0;
	if (debug)cout << " Cy_search " << cell_names[ca] << " "
		<< cell_names[cb] << " d " << digit + 1 << endl;
	nlim = n; step = 0;
	ych[0].Initcy(ca, cb,zeab, digit,n);// ca on cb off
}
void ERYY::YCH::Initcy(int eca, int ecb, BF128 cebf, int d,int n) {
	int debug = 0;
	SOLV81& p = solve.sv81w;
	memset(this, 0, sizeof ych[0]);
	// first ca off -> cb on (target)
	target = ecb; lim = n; digit = d;

	tcc.Set_c(eca); c1 = eca;
	tcc |= cebf;// be sure not to use elim
	(this + 1)->DoStepCy();	
}
void ERYY::YCH::DoStepCy() {
	if (seryy.iret)return;//finished
	int debug = 0;
	SOLV81& p = solve.sv81w;
	YCH& o = *(this - 1);	*this = o;	ispot++;	
	if (debug > 1)cout << "start ispot=" << ispot << " " << o.digit + 1 << " " << cell_names[o.c1] << endl;
	// New digit
	{
		register int v = p.cells[o.c1]; v &= ~(1 << o.digit);
		bitscanforward(digit, v);
		if (debug)cout << " new digit " << digit + 1 << endl;
	}
	// new biv cells seen by digit/cell, killed digits 
	BF128 seen= (p.dm[digit] & cell_z3x[o.c1]) & p.ccm[1];
	seen -= tcc; // no cell re used
	if (debug > 1)NameBf128List(" seen in step ", seen);
	if (seen.On_c(target)) {
		seryy.CyDoElims(ispot);
		return;
	}
	if (ispot >= lim) return;
	while ((c1 = seen.getFirstCell()) >= 0) {
		seen.Clear_c(c1); tcc.Set_c(c1);
		(this + 1)->DoStepCy();		
	}
}

void ERYY::CyDoElims(int ispot) {
#ifdef SEROUT
	cout << "seen y loop ispot= "<<ispot<<" ";
	for (int i = 0; i < ispot; i++) cout << cell_names[ych[i].c1]<<" ";
		cout << cell_names[cb] << endl;
#endif
	int debug = 0;
	SOLV81& p = solve.sv81w;
	iret = 1;
	p.Clean(digit, zeab);
	if(debug)NameBf128List("clean zeab", zeab);
	for (int i = 0; i < ispot; i++) {
		YCH& s = ych[i], sn = ych[i + 1];
		int d = sn.digit;
		BF128 e = (rclean1[d] & cell_z3x[sn.c1]) & cell_z3x[s.c1];
		if (e.isNotEmpty()) {
			solve.sv81w.Clean(d, e);
			if (debug) {
				cout << "clean digit " << d + 1;
				NameBf128List(" cells ", e);
			}
		}
	}

}

//==========================  y chain search
int SOLV81::DoEr6ych(int rat) {
	if (rat < 68 || rat>70) return 0;// safety
	int debug = 0;
	int nst[5] = { 3,5,7,9,20 },ns = nst[seryy.rating - 68],
		iret = 0;
	if (debug)cout << "DoEr6ych " << rat << " ns " << ns << endl;
	seryy.rating = rat;
	//memcpy(seryy.rclean1, slgybiv.rclean1, 9 * sizeof seryy.rclean1[0]);
	// rclean can have plenty of derived eliminations
	for (int id = 0; id < 9; id++) {
		BF128 wde = seryy.rclean1[id];
		if (wde.isEmpty()) continue;
		if (debug) { cout << " for digit " << id + 1;	NameBf128List(" elims ", wde); }
		seryy.digit = id; seryy.elimsd = wde;
		int ce;
		BF128 x = wde,	df2a= dm[id] & ccm[1],df2;
		while ((ce = x.getFirstCell()) >= 0) {
			x.Clear_c(ce);
			BF128 y = df2a & cell_z3x[ce];// seen cell biv
			int ca, cb ;
			while ((ca = y.getFirstCell()) >= 0) {
				y.Clear_c(ca);
				BF128 z = y;
				while ((cb = y.getFirstCell()) >= 0) {
					y.Clear_c(cb);
					BF128 we = (dm[id] & cell_z3x[ca]) & cell_z3x[cb];
					if ((we & wde) != we) continue;// all seen if an AIC is there
					// this can be the start Ychain
					if (debug) {
						cout << " y chain start " << id + 1 
							<< cell_names[ca] << " " << cell_names[cb];
						NameBf128List(" for elims ", we);
					}
					seryy.zeab = we;			seryy.iret = 0;
					seryy.Ch_search(ca, cb, ns);
					if (seryy.iret) {
						if (debug)cout << "seen chain ok " << cell_names[ca] << " " << cell_names[cb] << endl;
						// stop if new assign expected
						if ((df2a & we).isNotEmpty()) {
							serate.SetRating(rat);
							return 1;
						}
						iret++;
					}
				}
			}	
		}
	}	
	if (seryy.iret) 		serate.SetRating(rat);
	return iret;
}
void ERYY::Ch_search(int eca, int ecb, int n) {
	int debug = 0;
	ca = eca; cb = ecb;
	if (debug)cout << " Ch_search " << cell_names[ca] << " "
		<< cell_names[cb] << " d " << digit + 1 << endl;
	nlim = n; step = 0;
	ych[0].Initch(ca, cb, zeab, digit, n);// ca on cb off
}
void ERYY::YCH::Initch(int eca, int ecb, BF128 cebf, int d, int n) {
	int debug = 0;
	SOLV81& p = solve.sv81w;
	memset(this, 0, sizeof ych[0]);
	// first ca off -> cb on (target)
	target = ecb; lim = n; digit = d;

	tcc.Set_c(eca); c1 = eca;
	tcc |= cebf;// be sure not to use elim
	(this + 1)->DoStepCh();
}
void ERYY::YCH::DoStepCh() {
	if (seryy.iret)return;//finished
	int debug = 0;
	SOLV81& p = solve.sv81w;
	YCH& o = *(this - 1);	*this = o;	ispot++;
	if (debug > 1)cout << "start ispot=" << ispot << " " << o.digit + 1 << " " << cell_names[o.c1] << endl;
	// New digit
	{
		register int v = p.cells[o.c1]; v &= ~(1 << o.digit);
		bitscanforward(digit, v);
		if (debug)cout << " new digit " << digit + 1 << endl;
	}
	// new biv cells seen by digit/cell, killed digits 
	BF128 seen = (p.dm[digit] & cell_z3x[o.c1]) & p.ccm[1];
	seen -= tcc;// no cell re used
	if (debug > 1)NameBf128List(" seen in step ", seen);
	if (seen.On_c(target)) {
		seryy.ChDoElims(ispot);
		return;
	}
	if (ispot >= lim) return;
	while ((c1 = seen.getFirstCell()) >= 0) {
		seen.Clear_c(c1); tcc.Set_c(c1);
		(this + 1)->DoStepCh();
	}
}

void ERYY::ChDoElims(int ispot) {
#ifdef SEROUT
	cout << "seen y chain ispot= " << ispot << " ";
	for (int i = 0; i < ispot; i++) cout << cell_names[ych[i].c1]<<" ";
		cout << cell_names[cb] << endl;
#endif
	int debug = 0;
	SOLV81& p = solve.sv81w;
	iret = 1;
	p.Clean(digit, zeab);
	if (debug)NameBf128List("clean zeab", zeab);
}
