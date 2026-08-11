
/* pair/triplet exclusion
here is an example of pair exclusion in band 3
The process gives the same result as SE

457  156   2      |1367   9     13467  |8    35    57
78   3     1678   |5      1267  167    |679  4     279
9    56    4567   |367    23467 8      |367  235   1
 aligned pair exclusion for 5r7c1 using base cells r7c1 and r9c7 ;
 using excluding cells r7c8 ; r7c9 ; r9c2 ;
 5 r7c1can be seen not valid in a brute force analysis 
 using all 2/3 digits cells of the band as sets.
 r9c7 base and 5r7c1 target see r7c8 r7c9 r9c2 bi values 
 5 r7c1 is not possible.

 aligned triplet exclusion is coded in a similar way 
 and should give identical results to SE

 One biv cell not seen by the target is used to explain contradiction
 2 examples in the same puzzle band 3

13478  138    5     |1346  134   2     |347   67   9
1234*  9      13    |7     134   3456  |345*  8    2456
6      23*    37*   |3459  8     3459  |1     257* 2457*
 aligned triplet exclusion target  3r8c1 
 with 3 r8c1 r9c89 is 45 and r8c7 is 45


167    9    4    |1568  168   1568  |1357 2    378
12     3    5    |7     12489 1289  |6    14   489
8      27   167  |12456 3     12569 |1579 147  479
 aligned triplet exclusion target  4r8c9 or 4r9c9
 4r8c9 -> 1r8c8   2r8c1 7r9c2  r9c8  dead 
 SE base r8c9(target) plus r8c8 r9c2
 SE exclusion cells r8c1 r9c8

*/
struct SER62 {

	BF128 wbs, wbs2,wbs23; //band stack unsolved/pairs
	BF128 zbase,bs_base,xybase,xyzbase,xyseen,wbs23f; // as in skfr1
	int cb[3];// base cells for triplet
	BF128 wbsd, wbs2d,wbs23d; //same for digit d
	BF128 wa, wa2,wwseen,y;// same for cell ca
	SOLV81* ps;
	int ibs, id, ca;// band/stack  digit cella 
	int va,vax;
	int Pat_ca(SOLV81* s);
	int Go75bs(SOLV81* s);
	int Go75triplet();
	void Outa() {
		cout << "see bs " << ibs + 1 << " dig " << id + 1 << " ";
		NameBf128List(" pairs  ", wbs2d);
	}
	void Outb() {
		cout << "see bs " << ibs + 1 << " dig " << id + 1 << " "
			<< "  ca  " << cell_names[ca] ;
		NameBf128List(" seen  ", wwseen, 0);
		NameBf128List(" more y ", y);

	}
	void Outc() {
		cout << "see bs " << ibs + 1 << " dig " << id + 1 << " "
			<< "  ca  " << cell_names[ca] ;
		NameBf128List(" seen  ", wwseen, 0);
		NameBf128List(" y for more  ", y);

	}

}ser62;
int SER62::Pat_ca(SOLV81* s) {
	va = s->cells[ca];// contains the digit
	vax = va & ~(1 << id); // must see a cell in wa2
	wa = wbsd & cell_z3x[ca];
	wa2 = wa & wbs2d;
	if ((uint32_t)wa2.Count96() < _popcnt32(vax)) return 0;
	wwseen.SetAll_0();
	while (vax) {
		int d; bitscanforward(d, vax);
		vax ^= (1 << d);
		BF128 w = wa2 & s->dm[d];
		if (w.isEmpty())return 0;
		//cout << cell_names[w.getFirstCell()] << " added" << endl;
		wwseen |= w;
	}
	vax = va & ~(1 << id);
	if(va==vax)	y = wbsd - wwseen;
	else y=wa - wwseen;// last cell must see ca if contains digit
	y.Clear_c(ca);

	if (y.isEmpty())return 0;
	//Outc();
	
	// must be a y with wwseen in view
	while (vax) {
		int d,cc; bitscanforward(d, vax);
		vax ^= (1 << d);
		BF128 w = wa2 & s->dm[d], ws; ws.SetAll_0();
		// not sure to have only one cell here
		while ((cc = w.getFirstCell()) >= 0) {
			w.Clear_c(cc);
			ws |= cell_z3x[cc];
		}
		y &= ws;
	}
	if (y.isEmpty())return 0;
	Outb();
	s->Clean(id, y);
	serate.SetRating(62);
	return 1;
}
int SOLV81::DoEr62() {//aligned pair exclusion
	//cout << "entry new DoEr62()  "  << endl;
	int iret = 0;
	//cout << Char9out(serate.isbs23) << " possible ER62" << endl;
	for (  ser62.ibs = 0; ser62.ibs < 6; ser62.ibs++) {
		ser62.wbs = unsolved_cells & band3xBM[ser62.ibs];
		ser62.wbs2 = ser62.wbs & ccm[1];
		if (ser62.wbs2.Count96() < 3) continue;
		for (ser62.id = 0; ser62.id < 9; ser62.id++) {
			ser62.wbsd = dm[ser62.id] & ser62.wbs;
			ser62.wbs2d = ser62.wbsd & ccm[1];
			if (ser62.wbs2d.Count96() < 3) continue;
			//ser62.Outa();
			BF128 x = ser62.wbs - ser62.wbs2d;// one extra cell  
			while ((ser62.ca = x.getFirstCell()) >= 0) {
				x.Clear_c(ser62.ca);
				if (ser62.Pat_ca(this)) iret++;;
			}
		}
	}
	return iret;
}

int SOLV81::DoEr75() {//aligned pair exclusion
	cout << "entry new DoEr75()  "  << endl;
	int iret = 0;
	// as of skfr 
	for (ser62.ibs = 0; ser62.ibs < 6; ser62.ibs++) {
		ser62.wbs = unsolved_cells & band3xBM[ser62.ibs];
		ser62.wbs23 = ser62.wbs & (ccm[1]|ccm[2]);
		if (ser62.wbs23.Count96() < 3) continue;
		iret += ser62.Go75bs(this);
	}

	return 0;
	cout << Char9out(serate.isbs23) << " possible ER75" << endl;
	for (ser62.ibs = 0; ser62.ibs < 6; ser62.ibs++) {
		ser62.wbs = unsolved_cells & band3xBM[ser62.ibs];
		ser62.wbs2 = ser62.wbs & ccm[1];
		for (ser62.id = 0; ser62.id < 9; ser62.id++) {
			ser62.wbsd = dm[ser62.id] & ser62.wbs;
			ser62.wbs2d = ser62.wbsd & ccm[1];
			if (ser62.wbs2d.Count96() < 3) continue;
			//ser62.Outa();
			BF128 x = ser62.wbs - ser62.wbs2d;// one extra cell  
			while ((ser62.ca = x.getFirstCell()) >= 0) {
				x.Clear_c(ser62.ca);
				if (ser62.Pat_ca(this)) iret++;;
			}
		}
	}
	return iret;
}

int SER62::Go75bs(SOLV81* s) {
	ps = s; int iret = 0;
	zbase.SetAll_0();// zbase is all seen  from a wbs23 unsolved 
	{
		BF128 x = wbs23; register int c;
		while ((c = x.getFirstCell()) >= 0) {
			x.Clear_c(c);
			zbase |= cell_z3x[c];
		}
	}
	// loop on any pair of cell in zbase 
	BF128 x = zbase;	
	while ((cb[0] = x.getFirstCell()) >= 0) {
		x.Clear_c(cb[0]);
		BF128 y = x;
		BF128 xbase; xbase.SetAll_0(); xbase.Set_c(cb[0]);
		while ((cb[1] = y.getFirstCell()) >= 0) {
			y.Clear_c(cb[1]);
			xybase = xbase; xybase.Set_c(cb[1]);
			xyseen = ((wbs23 & cell_z3x[cb[0]]) | (wbs23 & cell_z3x[cb[1]])) - xybase;
			// we need to add an other base cell 
			// + 2 excludig cells that are visible by the 3 base cells
			if (xyseen.isEmpty()) continue;
			BF128 z = xyseen;
			while ((cb[2] = z.getFirstCell()) >= 0) {
				z.Clear_c(cb[2]);
				xyzbase = xybase; xyzbase.Set_c(cb[2]);
				// seen wbs23 minimum 2
				wbs23f = ((wbs23 & cell_z3x[cb[0]]) & cell_z3x[cb[1]]) & cell_z3x[cb[2]];
				if (wbs23f.Count() < 2)continue;
				iret+=Go75triplet();
			}
		}
	}
	return iret;
}
int SER62::Go75triplet() {
	// loop on permutation of potential candidate for the 3 base cells
	int te[27], nte = wbs23f.Table3X27(te);// excl cells in table
	int vv[3] = { 0,  0, 0 },vb[3],iret=0; // seen valid digits in base cells

	vb[2] = ps->cells[cb[2]];	int d3, bit3;//cb[2] has 2/3 digits 
	while (vb[2]) {
		bitscanforward(d3, vb[2]);	bit3= 1 << d3;	vb[2] ^= bit3;

		vb[0] = ps->cells[cb[0]];		int d1, bit1;
		if (tcellsrcb[cb[0]] & tcellsrcb[cb[2]])vb[0] &= ~bit3;
		while (vb[0]) {
			bitscanforward(d1, vb[0]);	bit1 = 1 << d1;	vb[0] ^= bit1;

			vb[1] = ps->cells[cb[1]]; int d2, bit2;
			if (tcellsrcb[cb[1]] & tcellsrcb[cb[2]])vb[1] &= ~bit3;
			if (tcellsrcb[cb[1]] & tcellsrcb[cb[0]])vb[1] &= ~bit1;
			while (vb[1]) {
				bitscanforward(d2, vb[1]);	bit2 = 1 << d2;	vb[1] ^= bit2;
				int digs3=bit1|bit2|bit3,aig = 1;
				for (int ite = 0; ite < nte; ite++) {
					register int ve = ps->cells[te[ite]];
					if ((ve & digs3) == ve) { aig = 0; break; }
				}
				if (aig) {// possible triplet
					vv[0] |= bit1; vv[1] |= bit2; vv[2] |= bit3;		}
			}
		}
	}
	for (int ibc = 0; ibc < 3; ibc++) {
		int c = cb[ibc], v0 = vb[ibc], vf = vv[ibc];
		if (vf != v0) {// one or more digits to clear in c
			ps->CleanCell(c, (v0 & ~vf));
			iret++;
		}
	}
	return iret;
}
