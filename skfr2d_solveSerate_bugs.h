
int SOLV81::BugCheck(BF128& m, int v) {
	BF128 x = m;
	int c, vc;
	while ((c = x.getFirstCell()) >= 0) {
		x.Clear_c(c);
		vc = cells[c] & ~v;
		if (_popcnt32(vc) > 2) return 0;
	}
	return 1;
}

struct SBUG {
	SOLV81* p;
	BF128 more,seenmore,seencom;
	int nm,ca,cb,cc,va,vb,vc,vcom,
		unitsa,unitsb,unitsc,unitsab,// ab is cum a,b,c
		units_1,units_m,// units with one or more "more"
		odd_or,odd_and,odda,oddb,oddc,odd2,nodd2,// odd2 is cum if c
		unc1,unc2,max3,debug;
	int vseen, vseenand,vand;
	void Init(SOLV81* pp) {
		p = pp;
		more = p->unsolved_cells - p->ccm[1];
		nm = more.Count96();		
	}
	int ApplyOneMore() {
		int cell = more.getFirstCell(), v = p->cells[cell];
		for (int i = 0, bit = 1; i < 9; i++, bit <<= 1)	if (bit & v)
			if ((p->dm[i] & p->unsolved_cells).Count96() & 1) {
				p->Assign(i, cell);
				serate.SetRating(56);
				return 1;
			}
		return 0;
	}
	int LockedDigit() {
		if (nm != 2)return 0; if (unc1 < 0)return 0;
		ca = more.getFirstCell(); cb = more.getLastCell();
		unitsab = tcellsrcb[ca] & tcellsrcb[cb];
		if (!unitsab) return 0;
		bitscanforward(unc1, unitsab);
		int dlock = -1;
		{
			BF128 w = p->unsolved_cells & units3xBM[unc1];
			int x = va, d;
			while (x) {
				bitscanforward(d, x);
				x ^= 1 << d;
				if ((w & p->dm[d]) == more) {
					dlock = d; break;
				}
			}
		}
		if (dlock < 0) return 0;
		if (debug)cout << "seen locked digit " << dlock + 1 << endl;
		// clean even in cross unit
		{
			int x = va ^ (1 << dlock), d, uxs = unitsa & ~unitsab, u1;
			bitscanforward(u1, uxs);
			while (x) {
				bitscanforward(d, x); x ^= 1 << d;
				if (!((p->dm[d] & units3xBM[u1]).Count96() & 1))
					p->Clear(d, ca);
			}
		}
		{
			int x = vb ^ (1 << dlock), d, uxs = unitsb & ~unitsab, u1;
			bitscanforward(u1, uxs);
			while (x) {
				bitscanforward(d, x); x ^= 1 << d;
				if (!((p->dm[d] & units3xBM[u1]).Count96() & 1))
					p->Clear(d, cb);
			}
		}
		serate.SetRating(57);
		return 1;
	}
	int ActiveDigit(int debug = 0) {
		if (debug) NameBf128List("entry Active digit more cells ", more);
		// find common cell and seen cells of "more"
		{
			vcom = 0xff;				units_1 = units_m = 0;
			seencom = p->unsolved_cells;
			BF128 x = more; register int c;
			while ((c = x.getFirstCell()) >= 0) {
				x.Clear_c(c); register int v = p->cells[c];
				if (_popcnt32(v) > 3)  return 0;				
				vcom &= v;
				seencom &= cell_z3x[c];
				v = tcellsrcb[c]; // units of c
				units_m |= v & units_1; units_1 |= v;
			}
			units_1 &= ~units_m;
			if (!vcom) return 0;
		}		
		// digit must be vcom and seen cell
		{
			BF128 x = seencom; register int c, vs = 0;
			while ((c = x.getFirstCell()) >= 0) {
				x.Clear_c(c); register int v = p->cells[c];
				if (v & vcom) vs |= v & vcom;
			}
			vcom = vs; if (!vcom) return 0;
			if (debug)cout << Char9out(vcom) << " vcom second active  " << endl;
		}
		// Take odd digit in units no other more
		{
			odd_or = 0; odd_and = vcom;
			BF128 x = more; register int c;
			while ((c = x.getFirstCell()) >= 0) {
				x.Clear_c(c);
				register int v = p->cells[c] & vcom,
					units = tcellsrcb[c] & units_1;
				if (!units)  return 0; // safety should not be
				{
					int ux; bitscanforward(ux, units);
					BF128 w = p->unsolved_cells & units3xBM[ux];
					register int odd = 0, v = vcom, d;
					while (v) {
						bitscanforward(d, v);
						int bit = 1 << d; v ^= bit;
						if ((p->dm[d] & w).Count96() & 1)		odd |= bit;
					}
					cout << Char9out(odd) << " odd for cell " << cell_names[c] << endl;
					if (!odd) return 0; 
					odd_or |= odd; odd_and &= odd;
				}
			}
		}
		if (odd_or |= odd_and) { cout << "not one odd" << endl; return 0; }
		int d; bitscanforward(d, odd_and);
		BF128 w = seenmore & p->dm[d];
		if (w.isEmpty()) return 0;
		cout << "BUG digit active  " << d + 1 << endl;
		p->Clean(d, w);
		serate.SetRating(57);
		return 1;
	}

	int  Init2() {	
		if (nm > 3) return 1;
		if (debug)NameBf128List(" more cells ", more);
		// find units and common unit
		{
			units_m = 0777777777;
			BF128 x = more; register int c;
			while ((c = x.getFirstCell()) >= 0) {
				x.Clear_c(c); 
				units_m &= tcellsrcb[c]; // units of c
			}
		}
		if (!units_m) return 1;// not same unit
		// Find odd digit(s) in each cell
		{
			odd2 = 0; 
			BF128 x = more; register int c;
			while ((c = x.getFirstCell()) >= 0) {
				x.Clear_c(c);
				register int v = p->cells[c] ,
					units = tcellsrcb[c] & ~units_m;
				int ux; bitscanforward(ux, units);
				BF128 w = p->unsolved_cells & units3xBM[ux];
				{
					register int odd = 0, d;
					while (v) {
						bitscanforward(d, v);
						int bit = 1 << d; v ^= bit;
						if ((p->dm[d] & w).Count96() & 1)		odd |= bit;
					}
					cout << Char9out(odd) << " odd for cell " << cell_names[c] << endl;
					if (!odd) return 0;
					odd2 |= odd; 
				}
			}
		}
		bitscanforward(unc1, units_m);

		nodd2 = _popcnt32(odd2);
		return 0;
	}	

}sbug;

int SOLV81::DoEr56() {// BUG
	int debug = 1; sbug.debug = 1;
	sbug.Init(this);
	if (sbug.nm > 4) return 0;
	if (sbug.nm == 1) { sbug.ApplyOneMore(); return 1; }
	if (sbug.LockedDigit()) return 1;
	if (sbug.ActiveDigit(debug)) return 1;
	if (sbug.Init2()) return 0;;
	if (debug)cout << " go for nak xx nodd2="<< sbug.nodd2 << endl;
	int iret = 0;
	if (sbug.nodd2 == 2) {
		iret += BugNakedPair(sbug.more,sbug.unc1,sbug.odd2);
		if(sbug.unc1!= sbug.unc2)
			iret += BugNakedPair(sbug.more, sbug.unc2, sbug.odd2);
		if (iret) return 1;		
	}
	if (sbug.nodd2 <= 3) {
		iret += BugNakedTriplet(sbug.more, sbug.unc1, sbug.odd2);
		if (sbug.unc1 != sbug.unc2)
			iret += BugNakedTriplet(sbug.more, sbug.unc2, sbug.odd2);
		if (iret) return 1;
	}	
	//if (sbug.nodd2 > 3) return 0; // waiting for examples
	if(debug)cout << "try quad" << endl;
	iret += BugNakedQuad(sbug.more, sbug.unc1, sbug.odd2);
	if (sbug.unc1 != sbug.unc2)
		iret += BugNakedQuad(sbug.more, sbug.unc2, sbug.odd2);
	if (iret) return 1;
	if (debug)cout << "try five" << endl;
	iret += BugNaked5A(sbug.more, sbug.unc1, sbug.odd2);
	if (sbug.unc1 != sbug.unc2)
		iret += BugNaked5A(sbug.more, sbug.unc2, sbug.odd2);
	if (iret) return 1;
	return 0;
}
int SOLV81::BugNakedPair(BF128& m,int unit, int v) {
	// look for nake pair
	int d1, d2, aig = 0;;
	bitscanforward(d1, v); bitscanreverse(d2, v);
	BF128 w = ((dm[d1] & dm[d2]) & ccm[1]) & units3xBM[unit];
	//cout << "entry naked pair " << d1 + 1 << d2 + 1 ;
	//NameBf128List(" cells pair d1d2 ", w);
	if (w.isNotEmpty()) {// we have a pseudo nake pair
		w |= m;// cells locked
		BF128 w2 = (dm[d1] - w) & units3xBM[unit];
		if (w2.isNotEmpty()) { Clean(d1, w2); aig = 1; }
		w2 = (dm[d2] - w) & units3xBM[unit];
		if (w2.isNotEmpty()) { Clean(d2, w2); aig = 1; }
		if (aig) { serate.SetRating(58);	return 1; }
	}
	return 0;
}
int SOLV81::BugNakedTriplet(BF128& m, int unit, int v) {
	//cout << "entry naked triplet sbug.nodd2 " << sbug.nodd2 << endl;
	if (sbug.nodd2 == 3)return BugNakedTripletB(m, unit, v);
	// look for nake triplet start 2 digits
	//char ws[82]; ws[81] = 0;
	//cout << "BugNakedTripletA "<<unit+1 << endl;
	int d1, d2, aig = 0;;
	bitscanforward(d1, v); bitscanreverse(d2, v);
	BF128 wd1 = (dm[d1] & ccm[1]) & units3xBM[unit],
		wd2 = (dm[d2] & ccm[1]) & units3xBM[unit];
	int l1, l2,v1;
	BF128 x = wd1;
	while ((l1 = x.getFirstCell()) >= 0) {
		x.Clear_c(l1);
		v1=cells[l1] ;
		//cout << Char9out(v1) << " v1 c=" << l1 + 1 << endl;
		BF128 y = wd2;
		while ((l2 = y.getFirstCell()) >= 0) {// no nake pair l1!=l2
			y.Clear_c(l2);
			if(! (cells[l2] & v1)) continue;
			if (l1 == l2) return 0;// safety
			// this is a nake triplet
			int d3f = v1 & ~v,d3, aig = 0;
			bitscanforward(d3, d3f);
			BF128 w; w.SetAll_0();
			w.Set_c(l1); w.Set_c(l2);
			BF128 wd3 = (dm[d3] - w) & units3xBM[unit];
			if (wd3.isNotEmpty()) { Clean(d3, wd3); aig = 1; }
			w |= m;// cells locked
			BF128 w2 = (dm[d1] - w) & units3xBM[unit];
			if (w2.isNotEmpty()) { Clean(d1, w2); aig = 1; }
			w2 = (dm[d2] - w) & units3xBM[unit];
			if (w2.isNotEmpty()) { Clean(d2, w2); aig = 1; }
			if (aig) { serate.SetRating(59);	return 1; }

		}
	}
	return 0;
}
int SOLV81::BugNakedTripletB(BF128& m, int unit, int v) {
	// look for nake triplet start 3digits 2/3 cells
	char ws[82]; ws[81] = 0;
	int d1, d2, d3,aig = 0;;
	bitscanforward(d1, v); bitscanreverse(d2, v);
	int vx = v ^ (1 << d1); bitscanforward(d3, vx);
	BF128 wd1 = (dm[d1] & ccm[1]) & units3xBM[unit],
		wd2 = (dm[d2] & ccm[1]) & units3xBM[unit],
		wd3 = (dm[d3] & ccm[1]) & units3xBM[unit];
	// no extra digit
	BF128 w = ((wd1 & wd2) | (wd1 & wd3)) | (wd2 & wd3);
	cout << sbug.nodd2 << " sbug.nodd2 entry naked tripletB  " 
		<<d1+1<<d2+1<<d3+1 ;
	NameBf128List(" cells to add ", w);
	if (w.Count96() != 2) return 0;
	cout << w.String3X(ws) << " seen nake triplet 3 digits in entry " << endl;
	BF128 wc = ((wd1 | wd2)| wd3) - w;
	BF128 w2 = dm[d1] & wc;
	if (w2.isNotEmpty()) { Clean(d1, w2); aig = 1; }
	w2 = dm[d2] & wc;
	if (w2.isNotEmpty()) { Clean(d2, w2); aig = 1; }
	w2 = dm[d3] & wc;
	if (w2.isNotEmpty()) { Clean(d3, w2); aig = 1; }
	if (aig) { serate.SetRating(59);	return 1; }
	return 0;
}
int SOLV81::BugNakedQuad(BF128& m, int unit, int v) {
	if (sbug.nodd2 == 3)return BugNakedQuadB(m, unit, v);
	if (sbug.nodd2 == 4)return BugNakedQuadC(m, unit, v);
	// look for nake 5 start 2 digits
	//cout<<Char9out(v) << "BugNakedQuad " << unit + 1 << endl;
	int d1, d2, aig = 0;;
	bitscanforward(d1, v); bitscanreverse(d2, v);
	BF128 wd1 = (dm[d1] & ccm[1]) & units3xBM[unit],
		wd2 = (dm[d2] & ccm[1]) & units3xBM[unit];
	int l1, l2, l3,  v1, v12, v123;
	BF128 x = (unsolved_cells & units3xBM[unit]) - m;
	//cout << x.String3X(ws) << " x " << endl;
	while ((l1 = x.getFirstCell()) >= 0) {
		x.Clear_c(l1);
		v1 = cells[l1];
		//cout << Char9out(v1) << " v1 c=" << cell_names[l1] << endl;
		BF128 y = x;
		if (y.Count96() < 2) return 0;
		while ((l2 = y.getFirstCell()) >= 0) {// no nake pair l1!=l2
			y.Clear_c(l2);
			v12 = v1 | cells[l2];
			//cout << Char9out(v12) << " v12 c=" << cell_names[l2] << endl;
			BF128 z = y;
			while ((l3 = z.getFirstCell()) >= 0) {// no nake pair l1!=l2
				z.Clear_c(l3);
				v123 = v12 | cells[l3];
				//cout << Char9out(v123) << " v123 c=" << cell_names[l3] << endl;
				if (_popcnt32(v123) != 4) continue;
				if ((v123 & v) != v) continue;
				// this is a nake quad
				//cout << "seen nake quad " << cell_names[l1] << " " << cell_names[l2]
				//	<< cell_names[l3]  << endl;
				// find all digits and try clean
				int dx[4], nx = 0, xd = v123, aig = 0;
				while (xd) {
					bitscanforward(dx[nx], xd);
					xd ^= 1 << dx[nx++];
				}
				BF128 w; w.SetAll_0();
				w.Set_c(l1); w.Set_c(l2);
				w.Set_c(l3); w |= m;
				for (int i = 0; i < 4; i++) {
					register int d = dx[i];
					BF128 wd = (dm[d] - w) & units3xBM[unit];
					if (wd.isNotEmpty()) { Clean(d, wd); aig = 1; }
				}
				if (aig) { serate.SetRating(60);	return 1; }
			}

		}
	}
	return 0;
}
int SOLV81::BugNakedQuadB(BF128& m, int unit, int v) {
	// look for nake quad start 3digits 2/3 cells
	//char ws[82]; ws[81] = 0;
	//cout <<Char9out(v)<< "BugNakedQuadB " << unit + 1 << endl;
	int d1, d2, d3, aig = 0;;
	bitscanforward(d1, v); bitscanreverse(d2, v);
	int vx = v ^ (1 << d1); bitscanforward(d3, vx);
	BF128 wd1 = (dm[d1] & ccm[1]) & units3xBM[unit],
		wd2 = (dm[d2] & ccm[1]) & units3xBM[unit],
		wd3 = (dm[d3] & ccm[1]) & units3xBM[unit];
	int l1, l2,l3, v1,v12,v123;
	BF128 x = (wd1|wd2)|wd3;
	while ((l1 = x.getFirstCell()) >= 0) {
		x.Clear_c(l1);
		v1 = cells[l1];
		//cout << Char9out(v1) << " v1 c=" << cell_names[l1]  << endl;
		BF128 y = x;
		while ((l2 = y.getFirstCell()) >= 0) {// no nake pair l1!=l2
			y.Clear_c(l2);
			v12=v1| cells[l2];
			if(v12==v)return 0;// safety no triplet
			//cout << Char9out(v12) << " v12 c=" << cell_names[l2] << endl;
			BF128 z = y;
			while ((l3 = z.getFirstCell()) >= 0) { 
				z.Clear_c(l3);
				v123 = (v12 | cells[l3]) & ~v;
				//cout << Char9out(v123) << " v123 c=" << cell_names[l3] << endl;
				if (!(_popcnt32(v123) == 1)) continue;
				// this is a nake quad
				int  d4;
				bitscanforward(d4, v123);
				if(0)
				cout << "seen nake quad " << cell_names[l1] << " " 
					<< cell_names[l2] << " " << cell_names[l3]
					<< " d4=" << d4 + 1 << endl;
				BF128 w; w.SetAll_0();
				w.Set_c(l1); w.Set_c(l2); w.Set_c(l3);
				w |= m;// cells locked
				BF128 wd4 = (dm[d4] - w) & units3xBM[unit];
				if (wd4.isNotEmpty()) { Clean(d4, wd4); aig = 1; }
				BF128 w2 = (dm[d1] - w) & units3xBM[unit];
				if (w2.isNotEmpty()) { Clean(d1, w2); aig = 1; }
				w2 = (dm[d2] - w) & units3xBM[unit];
				if (w2.isNotEmpty()) { Clean(d2, w2); aig = 1; }
				w2 = (dm[d3] - w) & units3xBM[unit];
				if (w2.isNotEmpty()) { Clean(d3, w2); aig = 1; }
				if (aig) { serate.SetRating(60);	return 1; }
			}

		}
	}
	return 0;
}
int SOLV81::BugNakedQuadC(BF128& m, int unit, int v) {
	// look for nake quad start 4 digits 2/3 cells
	BF128 wu = (unsolved_cells & units3xBM[unit]) - m,x=wu,wub=wu;
	int v1, cell;
	while ((cell = x.getFirstCell()) >= 0) {
		x.Clear_c(cell);
		v1 = cells[cell];
		if((v1&v)!=v1)wu.Clear_c(cell);// no extra digit
	}
	cout<< Char9out(v) << " nake quadC unit " << unit + 1;
	NameBf128List(" unsov fit ", wu);
	if (wu.Count96() != 3) return 0;// not a quad
	//================  we have a quad 
	int d1, d2, d3, d4, aig = 0;;
	bitscanforward(d1, v); bitscanreverse(d2, v);
	int vx = v ^ ((1 << d1) | (1 << d2));
	bitscanforward(d3, vx); bitscanreverse(d4, vx);
	BF128 wc = wub - wu;

	cout << sbug.nodd2 << " sbug.nodd2 entry naked quadc  "
		<< d1 + 1 << d2 + 1 << d3 + 1 << d4 + 1;
	NameBf128List(" cells to clean ", wub);
	BF128 w2 = dm[d1] & wc;
	if (w2.isNotEmpty()) { Clean(d1, w2); aig = 1; }
	w2 = dm[d2] & wc;
	if (w2.isNotEmpty()) { Clean(d2, w2); aig = 1; }
	w2 = dm[d3] & wc;
	if (w2.isNotEmpty()) { Clean(d3, w2); aig = 1; }
	w2 = dm[d4] & wc;
	if (w2.isNotEmpty()) { Clean(d4, w2); aig = 1; }
	if (aig) { serate.SetRating(60);	return 1; }
	return 0;
}
int SOLV81::BugNaked5A(BF128& m, int unit, int v) {
	if (sbug.nodd2 == 3)return BugNaked5B(m, unit, v);
	// look for nake 5 start 2 digits
	char ws[82]; ws[81] = 0;
	//cout<<Char9out(v) << "BugNaked5A " << unit + 1 << endl;
	int d1, d2, aig = 0;;
	bitscanforward(d1, v); bitscanreverse(d2, v);
	BF128 wd1 = (dm[d1] & ccm[1]) & units3xBM[unit],
		wd2 = (dm[d2] & ccm[1]) & units3xBM[unit];
	//cout << wd1.String3X(ws) << " wd1 " << endl;
	//cout << wd2.String3X(ws) << " wd2 " << endl;
	int l1, l2,l3,l4, v1,v12,v123,v4;
	BF128 x = (unsolved_cells & units3xBM[unit]) - m;
	//cout << x.String3X(ws) << " x " << endl;
	while ((l1 = x.getFirstCell()) >= 0) {
		x.Clear_c(l1);
		v1 = cells[l1];
		//cout << Char9out(v1) << " v1 c=" << l1 + 1 << endl;
		BF128 y = x;
		if (y.Count96() < 3) return 0;
		while ((l2 = y.getFirstCell()) >= 0) {// no nake pair l1!=l2
			y.Clear_c(l2);
			v12=v1| cells[l2];
			//cout << Char9out(v12) << " v12 c=" << l2 + 1 << endl;
			BF128 z = y;
			if (z.Count96() < 2) continue;
			while ((l3 = z.getFirstCell()) >= 0) {// no nake pair l1!=l2
				z.Clear_c(l3);
				v123 = v12 | cells[l3];
				//cout << Char9out(v123) << " v123 c=" << l3 + 1 << endl;
				BF128 q = z;
				while ((l4 = q.getFirstCell()) >= 0) {// no nake pair l1!=l2
					q.Clear_c(l4);
					v4 = v123 | cells[l4];
					//cout << Char9out(v4) << " v4 c=" << l4 + 1 << endl;
					if (_popcnt32(v4) > 5) continue;
					if ((v4 & v) != v) continue;
					// this is a nake 5
					//cout << "seen nake 5 " << l1 + 1 << " " << l2 + 1
					//	<< l3 + 1 << " " << l4 + 1 << endl;
					// find all digits and try clean
					int dx[5], nx = 0, xd = v4,aig=0;
					while (xd) {
						bitscanforward(dx[nx], xd);
						xd ^= 1 << dx[nx++];
					}
					BF128 w; w.SetAll_0();
					w.Set_c(l1); w.Set_c(l2);
					w.Set_c(l3); w.Set_c(l4); w |= m;
					for (int i = 0; i < 5; i++) {
						register int d = dx[i];
						BF128 wd = (dm[d] - w) & units3xBM[unit];
						if (wd.isNotEmpty()) { Clean(d, wd); aig = 1; }
					}					
					if (aig) { serate.SetRating(61);	return 1; }
				}
			}

		}
	}
	return 0;
}
int SOLV81::BugNaked5B(BF128& m, int unit, int v) {
	// look for nake 5 start 3digits 2/3 cells
	char ws[82]; ws[81] = 0;
	cout << Char9out(v) << "BugNaked5B " << unit + 1 << endl;
	int d1, d2, d3, aig = 0;;
	bitscanforward(d1, v); bitscanreverse(d2, v);
	int vx = v ^ (1 << d1); bitscanforward(d3, vx);
	BF128 wd1 = (dm[d1] & ccm[1]) & units3xBM[unit],
		wd2 = (dm[d2] & ccm[1]) & units3xBM[unit],
		wd3 = (dm[d3] & ccm[1]) & units3xBM[unit];
	int l1, l2, l3,l4, v1, v12, v123,v4;
	BF128 x = (unsolved_cells& units3xBM[unit])-m;
	NameBf128List(" unsolved - m ",x);

	while ((l1 = x.getFirstCell()) >= 0) {
		x.Clear_c(l1);
		v1 = cells[l1];
		cout << Char9out(v1) << " v1 c=" << cell_names[l1] << endl;
		if (x.Count96() < 3) continue;
		BF128 y = x;
		while ((l2 = y.getFirstCell()) >= 0) {// no nake pair l1!=l2
			y.Clear_c(l2);
			v12 = v1 | cells[l2];
			if (v12 == v)return 0;// safety no triplet
			cout << Char9out(v12) << " v12 c=" << cell_names[l2] << endl;
			if (y.Count96() < 2) continue;
			BF128 z = y;
			while ((l3 = z.getFirstCell()) >= 0) {
				z.Clear_c(l3);
				v123 = v12 | cells[l3];
				cout << Char9out(v123) << " v123 c=" << cell_names[l3] << endl;
				BF128 q = z;
				while ((l4 = q.getFirstCell()) >= 0) {
					q.Clear_c(l4);
					v4 = v123 | cells[l4] ;
					if (!(_popcnt32(v4) == 5)) continue;
					if (!((v4&v) == v)) continue;
					v4 &= ~v;
					// this is a nake 5
					int  d4,d5;
					bitscanforward(d4, v4); bitscanreverse(d5, v4);
					cout << "seen nake 5 " << cell_names[l1] << " " << cell_names[l2] << " " << cell_names[l3]
						<< " " << cell_names[l4] << " d45=" << d4 + 1 << d5 + 1 << endl;
					BF128 w; w.SetAll_0();
					w.Set_c(l1); w.Set_c(l2); w.Set_c(l3); w.Set_c(l4);
					w |= m;// cells locked
					BF128 w2 = (dm[d1] - w) & units3xBM[unit];
					if (w2.isNotEmpty()) { Clean(d1, w2); aig = 1; }
					w2 = (dm[d2] - w) & units3xBM[unit];
					if (w2.isNotEmpty()) { Clean(d2, w2); aig = 1; }
					w2 = (dm[d3] - w) & units3xBM[unit];
					if (w2.isNotEmpty()) { Clean(d3, w2); aig = 1; }
					w2 = (dm[d4] - w) & units3xBM[unit];
					if (w2.isNotEmpty()) { Clean(d4, w2); aig = 1; }
					w2 = (dm[d5] - w) & units3xBM[unit];
					if (w2.isNotEmpty()) { Clean(d5, w2); aig = 1; }
					if (aig) { serate.SetRating(61);	return 1; }
				}
			}

		}
	}
	return 0;
}
