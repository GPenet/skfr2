
//=========================================           solv81 functions

void SOLV81::Init(int * g) {
	memset(this, 0, sizeof SOLV81);
	g0 = g;
	unsolved_cells = bf128_3x;
	ccm[8] = bf128_3x;
	for (int i = 0; i < 9; i++)dm[i] = bf128_3x;
	for (int i = 0; i < 81; i++) cells[i] = 511;// all digits
	bug = 0;
}
void SOLV81::InitSlg(int* g) {
	memset(this, 0, sizeof SOLV81);
	g0 = g;
}
int SOLV81::Assign(int dig, int cell) {
#ifdef SEROUT
	if(serate.er>=SEROUT)
	cout << "tt-> assign " << dig + 1 << cell_names[cell] << endl;
#endif
	loop = 1;
	if(dig<0 || cell<0)return 1; // bug
	if (dig >8 || cell >80  )return 1; // bug
	if (dm[dig].Off_c(cell)) return 1; // bug
	BF128 w(cell_z3x[cell]); // cells seen by the target
	int* seen = seencells[cell];
	// cell solved and cell now one digit
	unsolved_cells.Clear_c(cell);
	register int  r = cells[cell], bit = 1 << dig; // current digit status of the cell
	r ^= bit;
	int ix = _popcnt32(r);// new count also index for old count
	ccm[ix].Clear_c(cell);
	ccm[0].Set_c(cell);
	cells[cell] = bit;
	// kill cell for other digits 
	const char* tx = t512v[r]; // list of digits in the cell 
	for (int i = 0; i < ix; i++) {
		register int j = tx[i] - '1';
		dm[j].Clear_c(cell);
	}
	// kill digit in seen cells adjust cells digits
	BF128 wd = dm[dig] & w; // seen active cells for the digit
	dm[dig] -= wd;
	for (int ic = 0; ic < 20; ic++) {
		int cell = seen[ic] - 1;
		if (wd.On_c(cell))cells[cell] &= ~(1 << dig);
	}
	// reduce the count by 1 for these cells
	if ((wd & ccm[0]).isNotEmpty()) return 1;// conflict now one empty cell
	for (int i = 1; i < 9; i++) {
		BF128 wwc = wd & ccm[i];
		ccm[i] -= wwc;
		ccm[i - 1] |= wwc;
	}
	return 0;
}
int SOLV81::Clear(int dig, int cell) {
	if (dm[dig].Off_c(cell)) return 1; // bug should never be
	dm[dig].Clear_c(cell);
	int  bit = 1 << dig; 
	cells[cell] ^= bit;
	int ix = _popcnt32(cells[cell]);// new count also index for old count
	if (!ix)return 1;// empty cell now should never be
	ccm[ix].Clear_c(cell);
	ccm[ix-1].Set_c(cell);
	return 0;
}
void SOLV81::Clean(int dig, BF128& c) {
	c &= dm[dig];// safety be sure to have them
	int tc[81], ntc = c.Table3X27(tc);
	for (int i = 0; i < ntc; i++)	Clear(dig, tc[i]);
}
void SOLV81::CleanDiag(int dig, BF128& c) {
	BF128 w; w.Diag3x27(c);
	Clean(dig, w);
}
void SOLV81::CleanCell(int cell,int digs ){
	for (int i = 0; i < 9; i++)
		if (digs & (1 << i))Clear(i, cell);
}

int SOLV81::GoSudoku(char* puz) {
	for (int i = 0; i < 81; i++)if (puz[i] != '.') {
		int dig = puz[i] - '1';
		if (dig < 0 || dig>8) return 1;
		if (Assign(dig, i)) return 1;		
	}
	return 0;
}
//________________ solve mode no contradiction
int SOLV81::DoSingles() {
	loop = 1;
	int cpt = 0;
	while (loop > 0) {
		if (unsolved_cells.isEmpty()) return 0;
		loop = 0;
		DoLastInCell();
		DoLastInUnit();
		if (bug) break;
	}
	return 0;

}
void SOLV81::DoLastInCell() {
	BF128 w = unsolved_cells & ccm[0];
	if (w.isEmpty())return;
	int tc[81], ntc = w.Table3X27(tc);
	for (int i = 0; i < ntc; i++) {
		int cell = tc[i], dig;
		bitscanforward(dig, cells[cell]);// get the dig
		if (Assign(dig, cell)) {
			bug = 1; return;
		}
	}
}
void SOLV81::DoLastInUnit() {
	for (int iu = 0; iu < 27; iu++) {
		BF128 w = unsolved_cells & units3xBM[iu];
		if (w.isEmpty())continue;
		if (w.Count() == 1) {// last cell in unit
			int cell = w.getFirstCell(), dig;
			bitscanforward(dig, cells[cell]);// get the dig
			if(Assign(dig, cell)){	bug = 1; return;}
			continue;
		}
		for (int id = 0; id < 9; id++) {
			BF128 wd = w & dm[id];
			if (wd.Count() == 1) {//digit  last cell in unit
				int cell = wd.getFirstCell();
				if (Assign(id, cell)) {
					bug = 1; return;
				}
				w &= unsolved_cells; // update w
			}
		}
	}
}
int SOLV81::DoLastInUnitMF(int f) {
	int iret = 0;
	for (int id = 0; id < 9; id++) {
		if (!(f & (1 << id))) continue; //not a floor digit
		//cout << "try digit " << id + 1 << endl;
		BF128 wd =  dm[id];
		for (int iu = 0; iu < 27; iu++) {
			BF128 wu = units3xBM[iu],wud=wu&wd;
			//cout << iu << " " << wud.Count() << " " << wud.Count96() << endl;
			if (wud.isEmpty()) return -1;// no room for the digit
			wud &= unsolved_cells ;
			if (wud.Count() == 1) {//digit  last cell in unit
				int cell = wud.getFirstCell();
				Assign(id, cell);
				iret++;
				//cout << "  ass digit unit " << id + 1 << " " << cell + 1 << " " << iu + 1 << endl;				Assign(id, cell); iret++;
				wd = dm[id]; // update wd
			}
		}
	}	
	return iret;
}


//________________ TE mode assign false then  look for contradiciton
int SOLV81::DoSinglesTE() {

	loop = 1;
	int cpt = 0;
	while (loop > 0) {
		if (unsolved_cells.isEmpty()) return 0;
		loop = 0;
		if (DoLastInCellTE()) return 1;;
		if(DoLastInUnitTE()) return 1;
	}
	return 0;
}
int  SOLV81::DoLastInCellTE() {
	BF128 w = unsolved_cells & ccm[0];
	if (w.isEmpty())return 0;
	int tc[81], ntc = w.Table3X27(tc);
	for (int i = 0; i < ntc; i++) {
		int cell = tc[i], dig,v= cells[cell];
		if (!v) return 1;		
		bitscanforward(dig, cells[cell]);// get the dig
		if (Assign(dig, cell)) return 1;		
	}
	return 0;
}
int SOLV81::DoLastInUnitTE() {
	for (int iu = 0; iu < 27; iu++) {
		BF128 w = unsolved_cells & units3xBM[iu];
		if (w.isEmpty())continue;
		if (w.Count() == 1) {// last cell in unit
			int cell = w.getFirstCell(), dig;
			bitscanforward(dig, cells[cell]);// get the dig
			if (Assign(dig, cell)) return 1;						
			continue;
		}
		for (int id = 0; id < 9; id++) {
			BF128 wd = w & dm[id];
			if (wd.Count() == 1) {//digit  last cell in unit
				int cell = wd.getFirstCell();
				//cout << " ldiu " << id + 1 << " " << cell + 1 << " " << iu + 1 << endl;
				if (Assign(id, cell)) return 1;				
				w &= unsolved_cells; // update w
			}
		}
	}
	return 0;
}
#ifdef SEROUT

//______________________ debugging code
void Linexx() {
	cout << '|';
	for (int i1 = 0; i1 < 3; i1++) {
		for (int j = 0; j < 30; j++) cout << '+';
		cout << '|';
	}
	cout << endl;
}
void SOLV81::ImageCandidats(int known) {
	Linexx();
	for (int ib = 0, j = 0; ib < 3; ib++) {
		for (int irow = 0; irow < 3; irow++) {
			cout << '|';
			for (int i1 = 0; i1 < 3; i1++) {
				for (int i2 = 0; i2 < 3; i2++, j++) {
					uint32_t v = cells[j];
					if (v >= 512) v = 0;// safety
					cout << " " << t512v[v];
				}
				cout << '|';
			}
			cout << endl;
		}
		Linexx();
	}
}

void SOLV81::ImageCandidatsShort() {
	// set column length
	char ws[110]; ws[109] = 0;
	char cl[9]; memset(cl, 2, sizeof cl);
	for (int i = 0; i < 81; i++) {
		int v = cells[i], col = i % 9, n = _popcnt32(v) + 1;
		if (n > cl[col])cl[col] = n;
	}
	int totl = 3;
	for (int i = 0; i < 9; i++)totl += cl[i];
	ws[totl] = 0;
	//cout << "totl =" << totl << endl;
	cout << endl;
	for (int ib = 0, j = 0; ib < 3; ib++) {
		for (int irow = 0; irow < 3; irow++) {
			cout << '|';
			for (int i1 = 0; i1 < 3; i1++) {
				for (int i2 = 0; i2 < 3; i2++, j++) {
					uint32_t v = cells[j], l = cl[j % 9];
					if (v >= 512) v = 0;// safety
					memcpy(ws, t512v[v], l);
					ws[l - 1] = ' ';// if 9 digits force one blank
					ws[l] = 0;
					cout << ws;
				}
				cout << '|';
			}
			cout << endl;
		}
		if (ib < 2) {
			memset(ws, '+', totl); cout << ws << endl;
		}
	}
	cout << endl << endl;
}


void Linenx(int n) {
	cout << '|';
	for (int i1 = 0; i1 < 3; i1++) {
		for (int j = 0; j < 3 * n; j++) cout << '+';
		cout << '|';
	}
	cout << endl;
}
void SOLV81::ImageMulti(int digs, int dummy, int known) {
	if (dummy & digs)return; // safety should not be
	if ((dummy | digs) > 0x1ff) return;// safety should not be
	int ncol = _popcnt32(digs) + 2;
	if (ncol > 9) return; // no reason to be here  > 7 digits
	int cellsw[81];
	memcpy(cellsw, cells, sizeof cellsw);
	char cdummy = t512v[dummy][0];
	for (int i = 0; i < 81; i++) {
		int c = cellsw[i] & digs;
		if (!c) { cellsw[i] = 0; continue; }
		if (c == cellsw[i]) continue;// no extra digit
		cellsw[i] = c | dummy;
	}
	Linenx(ncol);
	for (int ib = 0, j = 0; ib < 3; ib++) {
		for (int irow = 0; irow < 3; irow++) {
			cout << '|';
			for (int i1 = 0; i1 < 3; i1++) {
				for (int i2 = 0; i2 < 3; i2++, j++) {
					uint32_t v = cellsw[j];
					if (!(v & dummy)) {
						char w[10];
						memcpy(w, t512v[v], 10);
						w[ncol - 1] = 0;
						cout << " " << w;
					}
					else {
						v &= ~dummy;
						char w[10];
						memcpy(w, t512v[v], 10);
						w[ncol - 2] = 0;
						cout << " +" << w;
					}
				}
				cout << '|';
			}
			cout << endl;
		}
		Linenx(ncol);
	}
}


void SOLV81::ImageOne(int dig, int known) {
	BF128 w = dm[dig];
	if (!known) w &= unsolved_cells;
	for (int ib = 0, j = 0; ib < 3; ib++) {
		for (int irow = 0; irow < 3; irow++) {
			cout << ' ';
			for (int i1 = 0; i1 < 3; i1++) {
				for (int i2 = 0; i2 < 3; i2++, j++) {
					if (w.On_c(j)) cout << dig + 1;
					else cout << '.';
				}
				cout << ' ';
			}
			cout << endl;
		}
		cout << endl;
	}
}
#endif
