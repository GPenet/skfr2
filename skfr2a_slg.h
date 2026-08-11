#pragma once


struct SLG {// sets links group
	//============================ link field and results
	BF128 lfield[9], orf[9],allcells,cellslinks;
	BF128 rclean1[9];// back clean in serate mode 
	//============================ list of sets/links
	BF128 tsd[243], tld[243] ;// digit unit
	int tsc[80], * tcl, ntsc, ntlc, ntsd, ntld,
		digmulbf,digmul; // digit used for multi floors out (9 bits field)
	SOLV81  sv,svl;
	BF128 dsets[240],*tdl; // digits sets/lnks
	int dig1;// digit if one floor or dummy digit multi floors
	//=============================== process
	void InitFromSolve();
	void Initdigm(int dbf) {
		digmulbf = dbf;
		bitscanforward(digmul, dbf);
	}
	void InitLinks() { tdl = &dsets[ntsd]; tcl = &tsc[ntsc];	}
	void AddClink(int cell);
	void BuildLinkfield();
	//================================
	void NewSol(BF128* sol,int debug=0) {
		for (int i = 0; i < 9; i++) {
			char ws[82]; ws[81] = 0;
			orf[i] |= sol[i];
		}
	}
	int Clean(int seratemode=0); 
	int Clean1(int modeserate=0);
	int CleanMult();
	//===============================expand solve no triple point in sets
	int Expand1_sd_ld(int cc=0,int modeserate=0);// one floor rows sets all or column sets
	int Expand_sc_ld(int modeserate=0);// cells sets, digit lnks
	//=============== any mix general case
	int Expand_sc_ldm();// same multi floors
	//=============== any mix general case
	int Expand(int seratemode = 0);//  any mix of sets/links
	void Expand_Cells();
	void Expand_Digits();
	void Status(int mode);
	void DumpElims() {
		cout << "elims list from slg" << endl;
		for (int i = 0; i < 9; i++) if (rclean1[i].isNotEmpty()) {
			cout << i + 1;
			NameBf128List(" elims", rclean1[i]);
		}
	}
}slgybiv;
void SLG::Status(int mode ) {
	cout << "slg status  ntsc="<<ntsc << "  ntsd=" << ntsd
		<< endl;
	char ws[82]; ws[81] = 0;
	if (mode & 1) {// list of sets

		for (int i = 0; i < ntsc; i++) {
			int cell = tsc[i];
			cout << Char9out(sv.cells[cell]) <<" " << cell_names[cell] << endl;
		}
		for (int i = 0; i < ntsd; i++) {
			BF128 w = tsd[i];
			cout << " tsd dig " << w.bf.u32[3]+1;
			NameBf128List(" cells " , w);
		}
		for (int i = 0; i < 9; i++) {
			BF128 w = lfield[i];
			if (w.isNotEmpty())
				cout << w.String3X(ws) << " lf dig " << i + 1
				<< " " << w.Count96() << endl;

		}
		return;
		cout << cellslinks.String3X(ws)
			<< " cells as links for digits sets " << cellslinks.Count96() << endl;
	}
}

void SLG::InitFromSolve() {
	ntsc = ntsd = ntlc = ntld = 0;
	cellslinks.SetAll_0();
	sv = solve.sv81w;
	memset(rclean1, 0, 9 * sizeof  rclean1[0]);
}
void  SLG::AddClink(int cell) {
	allcells.Set_c(cell);
	cellslinks.Set_c(cell);
	int  v = sv.cells[cell];
	int ix = _popcnt32(v);
	const char* tx = t512v[v]; // list of digits in the cell
	for (int i = 0; i < ix; i++) {
		register int j = tx[i] - '1';
		lfield[j].Set_c(cell);
	}

}

void SLG::BuildLinkfield() {// and init sols
	memset(lfield, 0, sizeof lfield);
	memset(orf, 0, sizeof orf);
	// or of all linksets in lfield
	for (int i = 0; i < ntlc; i++) {
		int cell = tcl[i], v=sv.cells[cell];
		int ix = _popcnt32(v);
		const char* tx = t512v[v]; // list of digits in the cell
		for (int i = 0; i < ix; i++) {
			register int j = tx[i] - '1';
			lfield[j].Set_c(cell);
		}
	}
	for (int i = 0; i < ntld; i++) {
		BF128 w = tld[i];
		uint32_t d = w.bf.u32[3];
		w.bf.u32[3]=0;
		lfield[d]|=w;
	}
	if (0) {
		char ws[82]; ws[81] = 0;
			cout << "linkfield " << endl;
		for(int i=0;i<9;i++)
			if (lfield[i].isNotEmpty()) {
				cout << lfield[i].String3X(ws) << " dig " << i + 1 << endl;
			}
	}
}
int SLG::CleanMult() {
	int iret = 0;
	for (int i = 0; i < 9; i++) {
		BF128 w = lfield[i] - orf[i];
		//w &= solve.sv81w.dm[i]; // no double clean
		if (w.isEmpty())continue;
		//iret=1;		solve.sv81w.Clean(i, w);
		/*if (sgo.bfx[9] & 8)*/ {
			char ws[82]; ws[81] = 0;
			cout << w.String3X(ws) << " clean for digit " << i + 1 << endl;
		}
	}
	return iret;
}
int SLG::Clean(int modeserate) {
	char ws[82]; ws[81] = 0;
	int iret = 0;
	for (int i = 0; i < 9; i++) {
		BF128 w = lfield[i] - orf[i];
		w &= solve.sv81w.dm[i]; // no double clean
		if (w.isEmpty())continue;
		rclean1[i] = w; iret = 1;
		if (modeserate)continue;
		solve.sv81w.Clean(i, w);
	}
	return iret;
}

int SLG::Clean1(int modeserate) {
	BF128 w = lfield[0] - orf[0];
	w &= solve.sv81w.dm[dig1]; // no double clean
	if (w.isEmpty())return 0;
	if (modeserate) { rclean1[dig1] = w; return 1; }
	solve.sv81w.Clean(dig1, w);
	return 1;
}
//==================== expand one floor
int SLG::Expand1_sd_ld(int cc,int modeserate) {// one floor set in rows
struct SPOTs {
	BF128  pcells,field;
	int  cell, ispot;
	int GetCell() {
		cell = pcells.getFirstCell();
		if(cell<0) return -1;
		pcells.Clear_c(cell);
		if (field.Off_c(cell)) return -2;
		return cell;
	}
	void Godig() {	ispot++;field -= cell_z3x[cell];}
	void GodigCol() { ispot++; field -= Seencolbox[cell]; }

}spots[10],* s, * sp;	

	memset(spots, 0, sizeof spots[0]);
	memset(orf, 0, sizeof orf[0]);
	s = spots; s->pcells = tsd[0];
	memcpy(&s->field, lfield, sizeof lfield[0]);
	while (1) {
		register int cell = s->GetCell();
		if (cell == -2)continue;// dead
		if (cell<0) {	// end of spot
			if (s->ispot) { s--; continue; }
			if (orf[0].isEmpty()) 	return 2;			
			return Clean1(modeserate); 
		}
		sp = s; s++;// skip to next spot
		*s = *sp;
		if(cc)s->GodigCol();else s->Godig();
		if (s->ispot == ntsd) {// one sol
			orf[0] |= s->field;	s--; continue;	}
		s->pcells = tsd[s->ispot];		continue;
	}
	return 0;
}
//==================== expand one unit one band ...
struct SLGSPOTCELL {
	BF128 ass, dead, field[9];
	int pdigits, cell, dig, ispot;
	inline void Nextd() {
		bitscanforward(dig, pdigits);
		pdigits ^= 1 << dig;	}
	inline int Killed() {	return field[dig].Off_c(cell);	}
	int GetDig() {
		bitscanforward(dig, pdigits);
		pdigits ^= 1 << dig;	
		if (field[dig].Off_c(cell)) return 1;
		return 0;
	}
	void Godig(int debug=0) { ispot++;	
	if (debug) cout << dig + 1 << cell_names[cell] << " spot " << ispot << endl;
		field[dig] -= cell_z3x[cell]; }
	inline void Gom() { field[dig] -= cell_z3x[cell]; }
}spc[81];
int SLG::Expand_sc_ldm() {
	SLGSPOTCELL* s, * sp;
	memset(spc, 0, sizeof spc[0]);
	s = spc; s->cell = tsc[0];
	s->pdigits = sv.cells[s->cell];
	memcpy(s->field, lfield, sizeof lfield);
	cout << "start multi ntsc= "<<ntsc << endl;
	while (1) {
		if (!s->pdigits) {// end of cell
			if (s->ispot) { s--; continue; }
			return CleanMult();
		}
		s->Nextd();
		cout << s->ispot<<" "<<s->dig+1 << " " << s->cell+1 << endl;
		if(s->dig!=digmul && s->Killed()) continue;// dead digit
		sp = s; s++;// skip to next spot
		*s = *sp; s->ispot++;
		if (s->dig != digmul) 	s->Gom();
		if (s->ispot == ntsc) {// one sol
			cout << "one sol" << endl;
			NewSol(s->field);	s--; continue;
		}
		s->cell = tsc[s->ispot];
		s->pdigits = sv.cells[s->cell];
		continue;
	}
	return 0;
}
int SLG::Expand_sc_ld(int modeserate) {
	int debug = (modeserate > 1);
	if(debug)cout << "expand sc_ld mode " << modeserate << endl;
	//char ws[82]; ws[81] = 0;
	SLGSPOTCELL* s, * sp;
	memset(spc, 0, sizeof spc[0]);
	memset(orf, 0, sizeof orf);
	s = spc; s->cell = tsc[0];	s->pdigits = sv.cells[s->cell];
	memcpy(s->field, lfield, sizeof lfield);
	while (1) {
		if (!s->pdigits) {// end of cell
			if (s->ispot) { s--; continue; }	
			return Clean(modeserate);
		}
		if (s->GetDig()) continue;// dead digit
	
		sp = s; s++;// skip to next spot
		*s = *sp; s->Godig(debug);
		if (s->ispot == ntsc) {// one sol
			NewSol(s->field,debug);	s--; continue;	}
		s->cell = tsc[s->ispot];	s->pdigits = sv.cells[s->cell];
		continue;
	}
	return 0;
}
void SLG::Expand_Cells() {
	SLGSPOTCELL* s, * sp;
	memset(spc, 0, sizeof spc[0]);
	s = spc; s->cell = tsc[0];	s->pdigits = sv.cells[s->cell];
	memcpy(s->field, lfield, sizeof lfield);
	while (1) {
		if (!s->pdigits) {// end of cell
			if (s->ispot) { s--; continue; }
			return;// Clean();
		}
		if (s->GetDig()) continue;// dead digit
		sp = s; s++;// skip to next spot
		*s = *sp; s->Godig();
		if (s->ispot == ntsc) {// one sol
			NewSol(s->field);	s--; continue;
		}
		s->cell = tsc[s->ispot];	s->pdigits = sv.cells[s->cell];
		continue;
	}
}
void SLG::Expand_Digits() {// digit sets after cells or direct
	struct SPOTs {
		BF128  pcells, field;
		int  cell, ispot;
		int GetCell() {
			cell = pcells.getFirstCell();
			if (cell < 0) return -1;
			pcells.Clear_c(cell);
			if (field.Off_c(cell)) return -2;
			return cell;
		}
		void Godig() { ispot++; field -= cell_z3x[cell]; }
	}spots[10], * s, * sp;

	memset(spots, 0, sizeof spots[0]);
	memset(orf, 0, sizeof orf[0]);
	s = spots; s->pcells = tsd[0];
	memcpy(&s->field, lfield, sizeof lfield[0]);
	while (1) {
		register int cell = s->GetCell();
		if (cell == -2)continue;// dead
		if (cell < 0) {	// end of spot
			if (s->ispot) { s--; continue; }
			return ;
		}
		sp = s; s++;// skip to next spot
		*s = *sp; s->Godig();
		if (s->ispot == ntsd) {// one sol
			orf[0] |= s->field;	s--; continue;
		}
		s->pcells = tsd[s->ispot];		continue;
	}
}

int  SLG::Expand(int seratemode) {
	cout << "entry expand" << endl;
	Status(1);
	//if(!ntld)return Expand_sc_ld();

	return 0;
}
