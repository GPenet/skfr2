// working on sets
/*
for each digit 27 sets BF128
for each cell the 9 digits statsu
pointers to increasing size of sets

working set/links groups to find eliminations/assignments

struct SETS_LINKS {// find assigned/cleared
};

*/

void DSETS::Build(int dig, BF128& o)	 {
	d234m.SetAll_0();
	memset(rcb, 0, sizeof rcb);// clean obsolete data
	for (int iu = 0; iu < 27; iu++) {
		BF128 w = o & units3xBM[iu];
		int cc = w.Count();
		if (cc < 2)continue;// not expected singles unsolved
		cc -= 2; if (cc > 3)cc = 3;// bit in d234m
		d234m.Set(iu + 32 * cc);
		rcb[iu] = w;
	}


}

void SETS::Build(SOLVE& o) {
	for (int id = 0; id < 9; id++) {// 9 digits
		BF128 w = o.sv81w.dm[id] & o.sv81w.unsolved_cells; 
		ds[id].Build(id,w);
	}
	sv = &o.sv81w;
	//		ccm[9]; // cells map for count 'n'
	c2345[0]=sv->ccm[1];// 2 digs no empty no single
	c2345[1] = sv->ccm[2]; // 3 digs
	c2345[2] = sv->ccm[3]; // 4 digs
	c2345[3] = sv->ccm[4] |// 5 digs and more 
		sv->ccm[5] | sv->ccm[6] | sv->ccm[7] | sv->ccm[8];
	if (0) {// test dump
		char ws[82]; ws[81] = 0;
		cout << c2345[0].String3X(ws) << " pairs" << endl;
		cout << c2345[1].String3X(ws) << " 3" << endl;
		cout << c2345[2].String3X(ws) << " 4" << endl;
		cout << c2345[3].String3X(ws) << " more" << endl;
	}

}
