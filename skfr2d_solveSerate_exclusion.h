
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
	SLG slg;
	BF128 wbs, wbs2,wbs23,wbs4; //band stack unsolved/pairs
	BF128 wbs4c;
	BF128 zbase,bs_base,xybase,xyzbase,xyseen,wbs23f; // as in skfr1
	int cb[3];// base cells for triplet
	BF128 wbsd, wbs2d,wbs23d; //same for digit d
	BF128 wa, wa2,wwseen,y;// same for cell ca
	SOLV81* ps;
	int ibs, id, ca;// band/stack  digit cella 
	int va,vax;
	int cx, cy, cz, cz4, vx, vy, vz, vz4, vxy, vxyz, v4;
	int Pat_ca(SOLV81* s);
	int Go75(SOLV81* s);
	void AddCell(int c);
	int Go75bs(); int Go75bsB();
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
/* new triplet process
take in band stack 3 cells 2/3 digits max 4 digits
find all seen cells hit by digits
take one more cell same 4 digits to fill the nase
see if one cell sees all digit 'x' of the four
vx = ps->cells[cx];
*/
int SER62::Go75(SOLV81* s) {//aligned triplet exclusion
	//cout << "entry new DoEr75()  " << endl;
	ps = s;
	int iret = 0;
	for (ibs = 0; ibs < 6; ibs++) {
		wbs = ps->unsolved_cells & band3xBM[ibs];
		wbs23 = wbs & (ps->ccm[1] | ps->ccm[2]);
		if (wbs23.Count96() < 3) continue;
		wbs4 =  wbs & ps->ccm[3];
		iret += Go75bs();
	}
	if (iret) serate.SetRating(75);
	return iret;
}
int SER62::Go75bs() {
	//cout << "try bs " << ibs + 1 << endl;
	int iret = 0;
	{
		BF128 x = wbs23;   
		while ((cx = x.getFirstCell()) >= 0) {
			x.Clear_c(cx);	vx = ps->cells[cx];
			BF128 y = x;  
			while ((cy = y.getFirstCell()) >= 0) {
				y.Clear_c(cy);	vy = ps->cells[cy];
				vxy = vx | vy;
				if (_popcnt32(vxy) > 4) continue;
				BF128 z = y;  
				while ((cz = z.getFirstCell()) >= 0) {
					z.Clear_c(cz);	vz = ps->cells[cz];
					vxyz = vxy | vz;
					if (_popcnt32(vxyz) != 4) continue;
					//cell4 can be any cell same 2/4 digits
					BF128 z4 = z | wbs4; 
					while ((cz4 = z4.getFirstCell()) >= 0) {
						z4.Clear_c(cz4);	vz4 = ps->cells[cz4];
						v4 = vxyz | vz4;
						if (_popcnt32(v4) != 4) continue;
						iret += Go75bsB();
					}
				}
			}
		}
	}
	return iret;
}
void SER62::AddCell(int c) {
	slg.tsc[slg.ntsc++] = c;
	int v=  ps->cells[c],d;
	BF128 wl=wbs& cell_z3x[c];
	while (v) {
		bitscanforward(d, v); v ^= 1 << d;
		slg.lfield[d] |= ps->dm[d] & wl;
		slg.lfield[d].Set_c(c);// be sure to have single
	}

}

int SER62::Go75bsB() {// 4 cells possible seen quad
	// try each digit of v4 see if digits to clear
	slg.InitFromSolve();	memset(slg.orf, 0, sizeof slg.orf);
	AddCell(cx); AddCell(cy); AddCell(cz); AddCell(cz4);
	//slg.Status(1);
	if(slg.Expand_sc_ld(0)) {
#ifdef SEROUT
		wbs4c.SetAll_0(); wbs4c.Set_c(cx);
		wbs4c.Set_c(cy); wbs4c.Set_c(cz); wbs4c.Set_c(cz4);
		cout << Char9out(v4);	NameBf128List(" active four cells ", wbs4c);
		slg.DumpElims();
#endif
		return 1;
	}
	return 0;

}

int SOLV81::DoEr75(){//aligned triplet exclusion
	return ser62.Go75(this);
}
