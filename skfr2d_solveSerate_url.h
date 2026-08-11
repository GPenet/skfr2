

/* full processing for 2 cells with adds in one object UR or UL
   can be bivalue, "hidden locket set" or "locked set"
   always the lowest rating found
   the rule has been copied from SE code analysis adjusted to lksudokuffixed8 veersion.
   here "basis" is the rating entry for the specific object
	   UR basi=4.5
	   UL up to 6 cells basis=4.6 (+0.1)  7_8 cells 4.7 (+0.2)  10_.. 4.8 (+0.3)
   basis        one digit bivalue
   basis one digit active (same plus digit
   basis +0.1 (n-1) hidden/nacked sets of "n" cells

   for hidden and naked sets, the lowest rating is taken depending on "n" values

summary of rating having "equalled" hidden and naked as in lksudoku 1.2.5.0

 URUL
 cells -->  4    6    8  >=10

 direct   4.5  4.6  4.7  5.0

 pair     4.6  4.7  4.8  5.1    action 2
 triplet  4.7  4.8  4.9  5.2    action 3
 quad     4.8  4.9  5.0  5.3??  action 4
*/
struct UR_ULD {
	BF128 rcbiv, * rdm, runs, ccm_3, ccm_4;
	SOLV81* psv, svr; // pointer to svr and copie 
	uint32_t* rcells;
	int  digs, d1, d2;
	//=========== pair  status all, biv,3digits
	BF128 x2d, my2d, x3d;
	int actbs; // active bands stacks 
	//======= boxes analysis
	int ba,//all boxes with >= 2 cells actives
		ba_1,// one 2 biv r/c
		ba_2,// one 1 biv + 1.3 r/c
		ba_3, //one r/c 2x 3 digits 
		ba_4, //one r/c others
		ba_5,// any r/c 2 cells
		ba_6,// one cross 2 biv
		ba_7, // one cross other
		ba_8, // any cross 2 cells
		baz; // other mix of cells (10 items in total)
	int tboxorder[9], nboxorder, nboxorder1;// priority 1/2 r c cells pair
	void Dumpbx() {
		cout << "status box uruld  for pair " << d1 + 1 << d2 + 1 << endl;
		cout << Char9out(ba) << "  active box " << endl;
		if(ba_1)cout << Char9out(ba_1) << "  r/c pair 2 biv " << endl;
		if (ba_2)cout << Char9out(ba_2) << "  r/c pair 1 biv " << endl;
		if (ba_3)cout << Char9out(ba_3) << "  r/c pair 2x3 " << endl;
		if (ba_4)cout << Char9out(ba_4) << "  r/c pair other " << endl;
		if (ba_5)cout << Char9out(ba_5) << "  any r/c 2  " << endl;

		cout << "cross rc" << endl;
		if (ba_6)cout << Char9out(ba_6) << "  2 biv " << endl;
		if (ba_7)cout << Char9out(ba_7) << "  other " << endl;
		if (ba_8)cout << Char9out(ba_8) << "  all cross  " << endl;
		if (baz)cout << "> 2 cells" << endl
			<< Char9out(baz) << "  all mix  " << endl;

	}
	void Dumpborder() {
		cout<<Char9out(digs) << "digs   box order  "  ;
		for (int i = 0; i < nboxorder; i++)
			cout << tboxorder[i]+1 << " ";
		cout<< "nbo =" << nboxorder << endl;
	}

}uruld[36];



struct UL_SPOT {// one box
	BF128 zb, z2,// possible pairs biv not biv
		zass,//assigned cells
		z_more_valid,// restricted area for more
		z_one_valid, // same one digit more
		znext,z_one;
	uint32_t* rcells; // cells table
	int bass, bdead,ispot,
		isall,// if 1 rc if 2 only cross if 3 all
		rc18,rc2_18,nrc18, //useb once/twice  rc
		cumadds,isonemore,digs,moreone,
		tadd[4], nadd;// cells add max 4 one digit
	int boxes_to_see, // bit field 9
		bbx,// next boxes to see field 9
		ibox,ib9,// ibox 18-26 ib9  0-9
		ibs, 
		unit1,unit2, // next units to search if last box
		nboxtotake; // depth of the search for the rating
	int c1, c2, bseen_next,debug;
	void ThisIsURUL1();// one cell
	void ThisIsURUL2();// one digit active 
	void ThisIsURUL3();// one digit bi value
	void StoreIt();
	void ThisIsUL();// one// 1/2 units 

	void InitCom() {
		zass.SetAll_0(); z_more_valid.SetAll_1();
		z_one_valid = z_more_valid;
		cumadds = moreone = rc18=0;
	}
	int Addc(int c) {
		zass.Set_c(c);
		register int v= rcells[c];
		if (v == digs)return 0;
		v &= ~digs;// now more
		register int n = _popcnt32(v);
		if (n == 1) {// open to one digit active
			//cout << Char9out(v) << "addc one digit more " << c + 1;
			//cout <<" old moreone " << Char9out(moreone) << endl;
			if (moreone && v != moreone) {
				z_one_valid.SetAll_0();
				moreone = 3;// to force not =
			}
			else {
				if (!moreone) 	moreone = v;
				z_one_valid &= cell_z3x[c];
			}
		}
		z_more_valid &= cell_z3x[c];
		if(n>1) {// force stop single active
			z_one_valid.SetAll_0();
			moreone = 3;// to force not =
		}
		return (z_more_valid | z_one_valid).isEmpty();
	}

	//   URS 

	void StartUR(UR_ULD & s,int ib) {
		// debugging code to kill later
		char ws[82]; ws[81] = 0;
		InitCom();
		boxes_to_see =s.ba;
		ib9 = ib;	ibox =ib9 +18;
		BF128 w2 = zb & units3xBM[ibox],w=z2 & units3xBM[ibox];
		if (debug>1) {
			cout << " startur box " << ib+1 << endl;
			cout << w2.String3X(ws) << "UR w2 go for box " << ib9 + 1 << endl;
			cout << w.String3X(ws) << "UR w go for box " << ib9 + 1 << endl;
		}
		if (w2.isEmpty()) return ;// bug if
		boxes_to_see ^= 1 << ib9;
		c1 = w2.getFirstCell();
		UL_SPOT* sn = this + 1;

		if (w2.Count96() == 2) { 
			c2 = w2.getLastCell();
			*sn = *this;
			sn->GoNextBoxUR();
			return;
		}
		if (w.isNotEmpty()) {
			c2 = w.getFirstCell();
			*sn = *this;
			sn->GoNextBoxUR();
			return;
		}
		return;// not called with more than 2 cells 
		// box with more than 2 cells can be row &| col start
		int r = tcellsrcb3[c1][0], c = tcellsrcb3[c1][1];
		BF128 wrc = (z2 | zb) & units3xBM[ibox],
			ww = units3xBM[r];
		ww |= units3xBM[c]; wrc &= ww;
		while ((c2 = wrc.getFirstCell()) >= 0) {
			wrc.Clear_c(c2);
			*sn = *this;
			sn->GoNextBoxUR();
		}
	}
	void StartUR_bs(UR_ULD& s, int ib,int i_bs) {
		int debug = 0;
		InitCom();
		boxes_to_see = s.ba;
		ib9 = ib;	ibox = ib9 + 18;
		BF128 w2 = zb & units3xBM[ibox], w = z2 & units3xBM[ibox];
		if (debug > 1) {
			cout << " start bs ur in BS box " << ib + 1 ;
			NameBf128List(" biv in box ", w);
			NameBf128List(" cells with 2 digits  ", w2);
		}
		if (w2.isEmpty()) return;// bug if
		boxes_to_see &= ~( 1 << ib9); 
		int itrcb = i_bs / 3,  // row in band, col in stack
		  cell = w2.getFirstCell(),// unique
		  cross = tcellsrcb3[cell][1 - itrcb];
		c1 = cell;
		rc18 = 0;
		UL_SPOT* sn = this + 1;

		w &= units3xBM[cross];// can be 2 cells
		if (debug > 1)	NameBf128List(" cells for second ", w); 
		
		while ((c2 = w.getFirstCell()) >= 0) {
			w.Clear_c(c2);
			*sn = *this;
			sn->GoNextBoxUR();
		}

	}
	void GoNextBoxUR() {
		ispot++; zass.Set_c(c1);Addc(c2);// always valid
		Update_rc18();// 2 rows or 2 cols in next
		if (debug>1)cout << Char27out(rc18) << " UR entry last box " << c1 + 1 << " " << c2 + 1
			 << " ibox " << ibox + 1 << endl;

		bitscanforward(unit1, rc18); bitscanreverse(unit2, rc18);
		ibs = unit1 / 3;
		bbx= boxes_to_see & b9per_BS[ibs];

		while (bbx) { 
			bitscanforward(ib9, bbx);
			ibox = ib9 + 18;
			bbx ^= 1 << ib9;
			UL_SPOT* sn = this + 1;
			*sn = *this;
			sn->GetLastRC();	 			
		}
		return;
	}
	void GetLastRC();// last box start end row/col
	void GoLastRC();
	//================================== ULs
	inline void Update_rc18() {
		register int nr18 = tcellsrcb[c1] & 0777777,
			nc18 = tcellsrcb[c2] & 0777777,
			nrc18 = nr18 ^ nc18;// all singles rc
		rc2_18 = (nc18 & nr18) | rc18 & nrc18;
		rc18 ^= nrc18;
		if (debug > 1)cout << Char27out(rc18) << "  new status singles ispot= " << ispot << endl;
	}
	inline int GetNextBoxesToSee() {
		if (_popcnt32(rc18) > 2) {
			if (rc18 & 0777) {
				ibs = ib9 / 3;
				return  boxes_to_see & bseen_band[ib9];
			}
			else {
				ibs = (ib9 % 3) + 3;
				return  boxes_to_see & bseen_stack[ib9];
			}
		}
		// can have to go back use rc18 to get band xtack
		int rc;  bitscanforward(rc, rc18);
		ibs = rc / 3;
		return  boxes_to_see & b9per_BS[ibs];
	}
	inline void GoGetNextUL() {
		bitscanforward(ib9, bbx);
		ibox = ib9 + 18;
		UL_SPOT* sn = this + 1;
		*sn = *this;
		sn->GetNextUL();
	}
	void GetNextUL() {// recursive call
		char ws[82]; ws[81] = 0;
		int itrcb = ibs / 3;  // row in band, col in stack
		BF128 x = (z2 | zb) & units3xBM[ibox];// , zr = x;
		if (debug) {
			cout << "for ibox" << ibox + 1;	NameBf128List(" getnextUL x", x);
		}
		while ((c1 = x.getFirstCell()) >= 0) {
			x.Clear_c(c1);
			int unitx = tcellsrcb3[c1][itrcb],
				crossx = tcellsrcb3[c1][1 - itrcb];
			BF128 y = x;
			while ((c2 = y.getFirstCell()) >= 0) {
				y.Clear_c(c2);
				int unity = tcellsrcb3[c2][itrcb],
					crossy = tcellsrcb3[c2][1 - itrcb];
				if (unity == unitx) continue;
				if ((crossx == crossy)) {
					if (isall & 1) {
						UL_SPOT* sn = this + 1;
						*sn = *this;	sn->GoNextBoxUL();
					}
				}
				else if (isall & 2) {
					UL_SPOT* sn = this + 1;
					*sn = *this; sn->GoNextBoxUL();
				}
			}
		}
	}
	void GoNextBoxUL() {
		ispot++; zass.Set_c(c1); zass.Set_c(c2); 
		if (Addc(c1)) return;   	if (Addc(c2)) return;
		Update_rc18(); if (!rc18) 	return;
		int nx = _popcnt32(rc18);
		if (nx > 4 || nx<2) return; //out of the scope
		//if ((nrc18 + ispot) > nboxtotake) return;
		if (debug)cout << Char27out(rc18) << " entry next box UL " << c1 + 1 << " " << c2 + 1
			<< " ispot " << ispot << " ibox " << ibox + 1 << endl;
		ib9 = ibox - 18; boxes_to_see &= ~(1 << ib9);
		boxes_to_see &= ~(1 << ib9);
		bbx = GetNextBoxesToSee();// band or stack next
		if (debug>2) 	cout << Char9out(bbx) << " bbx  " << endl;
		
		// if last call same as for UR
		if (ispot == nboxtotake - 1) {
			if (_popcnt32(rc18) == 4) { GoLastUL4(); return; }
			bitscanforward(unit1, rc18); bitscanreverse(unit2, rc18);
			if (debug) cout << Char27out(rc18) << ">>>>>>>last units "<< unit1+1<< " " <<unit2+1  << endl;
			ibs = unit1 / 3;
			while (bbx) {
				bitscanforward(ib9, bbx);
				ibox = ib9 + 18;
				bbx ^= 1 << ib9;
				UL_SPOT* sn = this + 1;
				*sn = *this;
				sn->GetLastRC();
			}
		}
		else {
			isall = 3;
			while (bbx) { GoGetNextUL();bbx ^= 1 << ib9; }
		}
	}
	void GoLastUL4();
	void StartUL(UR_ULD& s, int ib) {
		InitCom();
		boxes_to_see = s.ba;
		ib9 = ib;	ibox = ib9 + 18;
		//start is always 2 cells in box
		BF128  w = (z2|zb) & units3xBM[ibox];
		if (debug > 1) 	cout << " startul box UL " << ib + 1 << endl;
		c1 = w.getFirstCell();c2 = w.getLastCell();
		UL_SPOT* sn = this + 1;
		*sn = *this;
		sn->GoNextBoxUL();
	}
	void StartULmix(UR_ULD& s, int ib,int ca,int cb) {
		InitCom();
		boxes_to_see = s.ba;
		ib9 = ib;	ibox = ib9 + 18;
		c1 = ca; c2 = cb;
		UL_SPOT* sn = this + 1;
		*sn = *this;
		sn->GoNextBoxUL();
	}

	//======================  4 boxes cross

	inline void GoGetNext8() {
		bitscanforward(ib9, bbx);
		ibox = ib9 + 18;
		boxes_to_see &= ~( 1 << ib9);
		UL_SPOT* sn = this + 1;
		*sn = *this;
		sn->GetNext8();
	}
	void GoNextBox8() {
		ispot++; zass.Set_c(c1); zass.Set_c(c2);
		if (Addc(c1)) return; if (Addc(c2)) return;
		Update_rc18();		
		if (debug)cout<< Char27out(rc18) << " entry next b8x " << c1 + 1 << " " << c2 + 1
			<< "  ispot " << ispot << endl;
		if (_popcnt32(rc18) != 4) return;
		if (ispot == 3) {GoLastUL4(); return;	}
		ib9 = ibox - 18; 
		boxes_to_see &= ~(1 << ib9);
		bbx = GetNextBoxesToSee();// band or stack next
		if (bbx) GoGetNext8();
		bbx ^= 1 << ib9;
		if (bbx)GoGetNext8();
	}
	void GetNext8() {// recursive call
		int itrcb = ibs / 3;  // row in band, col in stack
		BF128 x = (z2 | zb) & units3xBM[ibox];
		while ((c1 = x.getFirstCell()) >= 0) {
			x.Clear_c(c1);
			int unitx = tcellsrcb3[c1][itrcb],
				crossx = tcellsrcb3[c1][1 - itrcb];
			BF128 y = x;
			while ((c2 = y.getFirstCell()) >= 0) {
				y.Clear_c(c2);
				int unity = tcellsrcb3[c2][itrcb],
					crossy = tcellsrcb3[c2][1 - itrcb];
				if (unity == unitx) continue;
				if (crossx == crossy)  continue;
				UL_SPOT* sn = this + 1;
				*sn = *this;	sn->GoNextBox8();
			}	
		}
	}
	void StartUL8(UR_ULD& s, int ib) {
		InitCom();
		boxes_to_see = s.ba;
		ib9 = ib;	ibox = ib9 + 18;
		// start is always 2 cells in box
		BF128  w = (z2|zb) & units3xBM[ibox];
		if (debug > 1) {
			cout << " startul box U8L " << ib + 1 ;
			NameBf128List(" go for  ",w);
		}		
		c1 = w.getFirstCell(); c2 = w.getLastCell();
		UL_SPOT* sn = this + 1;
		*sn = *this;
		sn->GoNextBox8();

	}


}ulsp[20],rulast;// need 2 per box
struct UR_UL {
	BF128 rcbiv, * rdm, runs,ccm_3,ccm_4;
	SERATE* ps; // pointer to serate
	SOLVE * pso; // pointer to solve
	SOLV81* psv, svr; // pointer to svr and copie 
	UR_ULD uld,uld2;
	uint32_t* rcells;
	struct U2 {
		BF128 pat;
		int c3, c4, digs,d1,d2 , rating;
		inline void Add(BF128 p,int ec3, int ec4, int d , int r = 0) {
			pat = p;
			c3 = ec3; c4 = ec4; digs = d;  rating = r;
			bitscanforward(d1, digs); bitscanreverse(d2, digs);
		}

	}tu2[40];
	int ntu2,ibs,cell,digs,d1,d2,cellx;
	int active_ul, drstore, debug;
	int nuruld;
	//=========== pair 
	BF128 x2d, my2d,x2d3;
	int starts, // all starts for the pair
		strc, // same rows cols min 2 in rc
		strc1; // same no limit 
	char ws[82];// for debugging ws[81] = 0;


	int r, c,rc, // active row col from the band stack starts
		c1, c2, c3, c4; // cellsof the UR (UL) c1 corne if UR 
	// c3,c4 unit to see for extar digits
	//_________ cross start data
	int rc_c1, rc_c2;
	BF128 rcxx;// mask 2 cross rc less base to catch 2 cells active
	//=============== process 
	void Init_urul();
	int NewURUL(BF128& w) {
		for (int i = 0; i < ntu2; i++)
			if (w == tu2[i].pat) return 0;
		return 1;
	}

	void InitStep() {
		svr = *psv;
		rcbiv = svr.ccm[1];
		ccm_3 = svr.ccm[1] | svr.ccm[2];
		ccm_4 = ccm_3 | svr.ccm[3];
		runs = svr.unsolved_cells;
		rcells = svr.cells;
	}

	void Build_uruld() {
		debug = 0;
		InitStep();
		nuruld = 0;
		BF128 xx = rcbiv;
		while (xx.Count96() > 1) {
			UR_ULD & s=uruld[nuruld];
			// get and clean the pair
			{
				cell = xx.getFirstCell();
				s.digs = rcells[cell];
				bitscanforward(s.d1,s.digs);
				bitscanreverse(s.d2,s.digs);
				if (debug >1)
					cout << "getnextpair c " << cell + 1
					<< " ds " << s.d1 + 1 << s.d2 + 1 << endl;
				s.x2d = (rdm[s.d1] & rdm[s.d2]);// all unsolved cells
				s.my2d = s.x2d & rcbiv;
				s.x3d = s.x2d & ccm_3;
				xx -= s.my2d;

			}
			int nmy2d = s.my2d.Count96();
			int* bx = &s.ba;
			memset(bx, 0, 10 * sizeof bx[0]);
			// find boxes active 
			for (int ibx = 18, bit = 1; ibx < 27; ibx++, bit <<= 1) {
				BF128 w = s.x2d & units3xBM[ibx];
				int n = w.Count96();
				if (n>1) { 
					bx[0] |= bit;
					if (n == 2) {
						int c1 = w.getFirstCell(), c2 = w.getLastCell(),
							rcb= tcellsrcb[c1]| tcellsrcb[c2],
							nrc=_popcnt32(rcb);
						if (nrc == 4) {// mode row or col (1+2+1)
							bx[5] |= bit;
							BF128 w2 = w & s.my2d;
							if (w2 == w) { bx[1] |= bit; continue; }
							if(w2.isNotEmpty()) { bx[2] |= bit; continue; }
							// no bi value in the 2 cells
							w2 = w & s.x3d;
							if (w2 == w) {// is it same digit
								int c1 = w2.getFirstCell(),
									c2 = w2.getLastCell();
								if (rcells[c1] == rcells[c2]) {
									bx[3] |= bit; continue;
								}
							}
							{ bx[4] |= bit; continue; }
						}
						// now 2 cross cells
						bx[8] |= bit;
						BF128 w2 = w & s.my2d;
						if (w2 == w) { bx[6] |= bit; continue; }
						{ bx[7] |= bit; continue; }
					}
					else bx[9] |= bit;
				}
			}
			if (!bx[0]) continue;// nothing to do
			if (debug>1) s.Dumpbx();
			// check bands stack active
			int actbs = 0,allactb=0;
			for (int ibs = 0,bit=1; ibs < 6; ibs++,bit<<=1) {
				if (_popcnt32(b9per_BS[ibs] & s.ba) > 1) {
					actbs |= bit;
					allactb |= b9per_BS[ibs];
				}
			}
			if (!actbs) continue;
			s.actbs = actbs;
			if(debug>1)cout << Char9out(actbs) << " active bands stacks" << endl;;
			int usedb = allactb & s.ba;
			if (usedb != s.ba) {
				if(debug>1)cout << Char9out(s.ba & ~usedb) << " unused boxes" << endl;
				for (int i = 0; i < 10; i++)	bx[i] &= usedb;
			}
			s.nboxorder = 0;
			int  x = bx[1],y;
			while (x) {// 2 bi values
				bitscanforward(y, x); x ^= 1 << y;
				s.tboxorder[s.nboxorder++] = y;
			}
			x = bx[2];
			while (x) {//one bi value
				bitscanforward(y, x); x ^= 1 << y;
				s.tboxorder[s.nboxorder++] = y;
			}
			x = bx[9];
			while (x) {//box complex one biv one or more r c
				bitscanforward(y, x); x ^= 1 << y;
				BF128 w = s.x2d & units3xBM[y+18],w2 = w& s.my2d;
				if (w2.isEmpty()) continue;
				int cell = w2.getFirstCell();
				int r= tcellsrcb3[cell][0],c= tcellsrcb3[cell][1];
				BF128 wx = units3xBM[r]; wx |= units3xBM[c];
				w2 &= wx;// cells that can be row or col
				if( w2.Count96()>1)
				 s.tboxorder[s.nboxorder++] = y;
			}
			s.nboxorder1 = s.nboxorder;
			x = bx[3];
			while (x) {//2 triplets same digit
				bitscanforward(y, x); x ^= 1 << y;
				s.tboxorder[s.nboxorder++] = y;
			}
			if(debug)s.Dumpborder();
			nuruld++;
		}
		if(debug)cout << "end build n pairs to see " << nuruld << endl;
	}

	int GetNextpair() {
		while (rcbiv.Count96() > 1) {
			// get and clean the pair
			cell = rcbiv.getFirstCell();
			digs = rcells[cell];
			bitscanforward(d1, digs);
			bitscanreverse(d2, digs);
			if(debug>1)
			cout << "getnextpair c " << cell + 1 
				<< " ds " << d1 + 1 << d2 + 1 << endl;
			x2d = (rdm[d1] & rdm[d2]);// &runs;
			my2d = x2d & rcbiv;
			rcbiv -= my2d;
			int nmy2d = my2d.Count96();
			// need 2 cells in row or col get r/c/b
			BF128 x = my2d;
			register int m1 = 0, m2 = 0;
			while ((cellx = x.getFirstCell()) >= 0) {
				x.Clear_c(cellx);
				register int m = tcellsrcb[cellx];
				m2 |= m1 & m; m1 |= m;
			}
			starts = m2; strc = m2 & 0777777;
			strc1= m1 & 0777777;
			if (strc1)return 1;
		}
		return 0;
	}
	// active digit in UR UL one extra digit
	int Check2() {
		int v1 = svr.cells[c3] & ~digs, v2 = svr.cells[c4] & ~digs;
		if (v1 == v2 && _popcnt32(v1) == 1) {// posssible active digit
			int d; bitscanforward(d, v1);
			BF128 x = svr.dm[d] & cell_z3x[c3]; x &= cell_z3x[c4];
			if (x.isNotEmpty()) {
				if(debug)cout << "active single digit" << endl;
				psv->Clean(d, x);
				return 1;
			}
		}
		return 0;
	}

	void  InitDoElSearch(UR_ULD& s, int nbox);
	int DoEr45() {// UR UL base
		debug = 0;
		if (debug) 	cout << "urul do er45" << endl;
		drstore = ntu2 = active_ul=0;// init store for hn pairs...
		int iret = 0;
		for (int iurd = 0; iurd < nuruld; iurd++) {
			uld= uruld[iurd];
			InitDoElSearch(uld, 2);
			if(debug>1)cout << Char9out(uld.digs) << "n to see" << uld.nboxorder1 << endl;
			int nb=_popcnt32(uld.ba);
			if (uld.nboxorder1) {
				for (int ist = 0; ist < uld.nboxorder1; ist++) {
					int ib9 = uld.tboxorder[ist];
					ulsp[0].StartUR(uld, ib9);
					uld.ba &= ~(1 << ib9); // no redundancy
					nb--;
					if (nb < 2) break;
				}
			}
			if (nb > 1 && uld.baz) {
					// must be in a band or a stack 2 boxes
				for (int ibs = 0; ibs < 6; ibs++) {
					int x= uld.ba & b9per_BS[ibs];
					if (_popcnt32(x) > 1) {
						uld2 = uld;// use a new uld
						uld2.ba = x;
						uld2.baz &= x;
						uld2.nboxorder = 0;
						if (debug) {
							cout << ibs + 1 << " a band stack to check after easy" << endl;
							cout << Char9out(x) << " boxes to study" << endl;
							cout << Char9out(uld2.baz) << " uld2.baz" << endl;
						}
						// first  must have a bi value 
						for (int i2 = 0, bit = 1; i2 < 9; i2++, bit <<= 1) {
							if (!(uld2.baz & bit))continue;
							BF128 w = uld2.my2d & units3xBM[i2 + 18];
							if (w.isNotEmpty()) {
								uld2.tboxorder[uld2.nboxorder++] = i2;
							}
						}
						if (debug)uld2.Dumpborder();
						if (uld2.nboxorder) {
							int nb = _popcnt32(uld2.ba);
							for (int ist = 0; ist < uld2.nboxorder; ist++) {
								int ib9 = uld2.tboxorder[ist];
								ulsp[0].StartUR_bs(uld2, ib9, ibs);
								if (nb-- < 3) break;
								uld2.ba &= ~(1 << ib9); // no redundancy
							}
						}

					}


				}

			}

		}
		if (active_ul) serate.SetRating(45);
		return active_ul;
	}	

	void Do4xA(int n_box) {
		//================================== new ULs 3 boxes
		for (int iurd = 0; iurd < nuruld; iurd++) {
			uld = uruld[iurd];
			int nb = _popcnt32(uld.ba);
			if (nb < n_box) continue;;
			InitDoElSearch(uld, n_box);
			if (debug > 1)cout << Char9out(uld.digs) << "n to see " << uld.nboxorder << endl;
			if (uld.nboxorder) {// including start 2x3 digits
				for (int ist = 0; ist < uld.nboxorder; ist++) {
					int ib9 = uld.tboxorder[ist];
					uld.ba &= ~(1 << ib9); // no redundancy
					ulsp[0].StartUL(uld, ib9);
					nb--;
					if (nb < n_box) break;
				}
			}
			if (nb > 1 && uld.baz) {
				if (debug) {
					cout << Char9out(uld.ba) << " enter do4xa mix box" << endl;
					cout << Char9out(uld.baz) << " baz" << endl;
				}
				// must be in a band or a stack 2 boxes
				for (int ibs = 0; ibs < 6; ibs++) {
					int x = uld.ba & b9per_BS[ibs], isstack = ibs / 3;
					if (_popcnt32(x) > 1) {
						if (debug) {
							cout << ibs + 1 << " a band stack to check after easy" << endl;
							cout << Char9out(x) << " boxes to study" << endl;
						}
						// take a box possible start
						for (int i2 = 0, bit = 1; i2 < 9; i2++, bit <<= 1) {
							if (!(uld.baz & bit))continue;
							uld.ba &= ~bit;// kill the box for ather search
							BF128 w = (uld.x2d & units3xBM[i2 + 18])&ccm_3;
							if (w.isEmpty()) continue;// should never be
							if (debug) {
								cout << "box " << i2 + 1 << " in band stack " << ibs + 1;
								NameBf128List(" cells in box for a start", w);

							}
							int c0 = 27 * (i2 / 3) + 3 * (i2 % 3),// first cell
								j1=(isstack)?0:9;
							for (int j=j1; j < j1+9; j++) {
								int c1 = c0 + t2_18[j][0], c2 = c0 + t2_18[j][1];
								if (w.Off_c(c1)  || w.Off_c(c2)) continue;
								//cout << " try " << cell_names[c1] << " " << cell_names[c2] << endl;
								uld2 = uld;// use a new uld
								ulsp[0].StartULmix(uld2, i2, c1, c2);

							}					

						}
					}
				}
			}
		}
	}

	int DoEr46() {// get ul then work on the file
		InitStep();
		drstore = 1;
		active_ul = PendingURUL(46);
		if (active_ul) {
			serate.SetRating(46); return 1;	}
		Do4xA(3);
		if (active_ul) serate.SetRating(46);
		return active_ul;
	}
	int DoEr47() {// get ul then work on the file
		InitStep();
		drstore = 2;
		active_ul = PendingURUL(47);
		if (active_ul) { serate.SetRating(47); return 1; }
		Do4xA(4);
		// now four cross boxes if any					
		for (int iurd = 0; iurd < nuruld; iurd++) {
			uld = uruld[iurd];
			if (!uld.ba_8) continue;
			uld.ba &= ~uld.ba_5;
			if (_popcnt32(uld.ba) < 4)continue;
			if (!uld.ba_6) continue;// need at least one out of 4
			InitDoElSearch(uld, 4);
			int nb = _popcnt32(uld.ba),ib9;
			while (uld.ba_6) {
				bitscanforward(ib9, uld.ba_6);
				uld.ba &= ~(1 << ib9); // no redundancy
				uld.ba_6 &= ~(1 << ib9);  
				ulsp[0].StartUL8(uld, ib9);
				nb--;
				if (nb < 4) break;
			}
		}
		if (active_ul) serate.SetRating(47);
		return active_ul;
	}
	int DoEr48() {// get ul then work on the file
		InitStep();
		active_ul = PendingURUL(48);
		if (active_ul) { serate.SetRating(48); return 1; }
		return active_ul;
	}
	int DoEr49() {// get ul then work on the file
		InitStep();
		active_ul = PendingURUL(49);
		if (active_ul) { serate.SetRating(49); return 1; }
		return active_ul;
	}	
	int DoEr50() {// get ul then work on the file
		InitStep();
		drstore = 5;
		active_ul = PendingURUL(50);
		if (active_ul) { serate.SetRating(47); return 1; }
		Do4xA(5);		
		if (!active_ul)Do4xA(6);	if (!active_ul)Do4xA(7);
		if (!active_ul)Do4xA(8);	if (!active_ul)Do4xA(9);
		if (active_ul) serate.SetRating(50);
		return active_ul;
	}
	int DoEr51() { 
		InitStep();
		active_ul = PendingURUL(51);
		if (active_ul)  serate.SetRating(51);
		return active_ul;
	}
	int DoEr52() {
		InitStep();
		active_ul = PendingURUL(52);
		if (active_ul)  serate.SetRating(52);
		return active_ul;
	}
	int DoEr53() {// never seen ??
		InitStep();
		active_ul = PendingURUL(53);
		if (active_ul)  serate.SetRating(53);
		return active_ul;
	}
	int PendingURUL(int r) {// r=0 if 46 1 if 47 ...
		r -= 45;
		int tgo[50][2], ntgo = 0;
		for (int i = 0; i < ntu2; i++) {
			U2& x2 = tu2[i];// here all URs with more 
			register int iu = tcellsrcb3[x2.c3][2];
			if (tcellsrcb3[x2.c4][2] == iu)	{// check box if
				tgo[ntgo][0] = i;
				tgo[ntgo++][1] = iu;
			}
			iu = tcellsrcb3[x2.c3][1];
			if (tcellsrcb3[x2.c4][1] == iu) {// check col if
				tgo[ntgo][0] = i;
				tgo[ntgo++][1] = iu;
			}
			iu = tcellsrcb3[x2.c3][0];
			if (tcellsrcb3[x2.c4][0] == iu) {// check row if
				tgo[ntgo][0] = i;
				tgo[ntgo++][1] = iu;
			}
		}		int iret = 0;
		// look in pending for rating 46
		register int ir = 0;
		for (int igo = 0; igo < ntgo; igo++) {
			register int iu = tgo[igo][1],i= tgo[igo][0];
			U2 & x2 = tu2[i];// here all URs with more 
			int dr = r - x2.rating;// should always be >=1
			if (dr < 1 || dr>4) continue; // safety
			switch (dr) {
			case 1: // naked or hidden pair
				ir += Iel2(i, iu) + IelH2(i, iu);	break;
			case 2: // naked hidden triplet 
				ir += IelH3(i, iu) + IelN3(i, iu); //a(i, iu) + IelN3b(i, iu);
				break;
			case 3: // naked hidden quad 
				ir += IelH4(i, iu) + IelN4(i, iu);
				break;
			}
		}
		return ir;
	}
	int Iel2(int iu2, int iu) {
		U2& u2 = tu2[iu2];
		int more = (rcells[u2.c3] | rcells[u2.c4]) & ~u2.digs;
		if (_popcnt32(more) != 2) return 0;
		BF128 w34; w34.SetAll_0(); w34.Set_c(u2.c3); w34.Set_c(u2.c4);
		int md1, md2;
		bitscanforward(md1, more); bitscanreverse(md2, more);
		BF128 w = (rdm[md1] | rdm[md2]) & units3xBM[iu];
		// is it naked pair
		BF128 w2 = (rdm[md1] & rdm[md2]) & units3xBM[iu];
		w2 &= rcbiv;
		if ((w2 & rcbiv).isEmpty()) return 0;//no naked pair
		int nn = w.Count96();
		if (nn < 4) return 0;
		w -= (w2 | w34);
		BF128 wa = w & rdm[md1], wb = w & rdm[md2];
		if (wa.isNotEmpty()) 	psv->Clean(md1, wa);
		if (wb.isNotEmpty()) 	psv->Clean(md2, wb);
		return 1;

	}
	int IelH2(int iu2, int iu) {
		U2& u2 = tu2[iu2];
		BF128 w = (rdm[u2.d1] | rdm[u2.d2]) & units3xBM[iu];
		if (w.Count96() == 3) {// hidden x
			w.Clear_c(u2.c3); w.Clear_c(u2.c4);
			int cx = w.getFirstCell(), v = rcells[cx] & ~u2.digs;
			if (v) {
				psv->CleanCell(cx, v);
				return 1;
			}
		}
		return 0;
	}
	int IelH3(int iu2, int iu) {
		U2& u2 = tu2[iu2];
		BF128 w = (rdm[u2.d1] | rdm[u2.d2]) & units3xBM[iu];
		int nn = w.Count96();
		if (nn == 4 || nn == 3) {// hidden 3e if on locked digit
			int alld = rcells[u2.c3] | rcells[u2.c4],
				more = alld & ~u2.digs;
			w.Clear_c(u2.c3); w.Clear_c(u2.c4);
			int cx = w.getFirstCell(), v = rcells[cx], vv = v, cy;
			if (nn == 4) {
				cy = w.getLastCell();
				v |= rcells[cy]; vv &= rcells[cy];
			}
			int nn2 = _popcnt32(v);
			if (nn2 < 4) return 0; // nothing to do
			int v2 = vv & ~alld;// look for an extra digit bi value
			if (!v2) return 0;
			//cout << Char9out(v2) << "possible extra digit for a triplet" << endl;
			for (int i = 0, bit = 1; i < 9; i++, bit <<= 1) {
				if (!(bit & v2)) continue;
				BF128 wd = rdm[i] & units3xBM[iu];
				if (!(wd.Count96() == 2)) continue;
				if (nn == 4 && wd.Off_c(cy))continue;
				int keepd = u2.digs | bit;
				if (nn == 3) {
					wd.Clear_c(cx); cy = wd.getFirstCell();
				}
				int x = rcells[cx] & ~keepd;
				if (x)psv->CleanCell(cx, x);
				x = rcells[cy] & ~keepd;
				if (x)psv->CleanCell(cy, x);
				return 1;
			}
		}
		return 0;
	}
	int IelH4(int iu2, int iu) {
		U2& u2 = tu2[iu2];
		BF128 x12 = runs & units3xBM[iu];
		x12.Clear_c(u2.c3); x12.Clear_c(u2.c4);
		BF128 wn = x12;// all cells not c3 c4
		int alld = rcells[u2.c3] | rcells[u2.c4],
			more = alld & ~u2.digs, cell;
		BF128 x = x12; 
		int vcum = 0;
		while( (cell = x.getFirstCell()) >= 0){
			x.Clear_c(cell);
			register int v = rcells[cell];
			if (!(v & u2.digs))	x12.Clear_c(cell);	
			else vcum |= v;
		}
		int vcmore = vcum & ~u2.digs; 
		vcmore &= ~more;// can not be a more digit
		if (x12.Count96() > 3) return 0;// no hidden quad
		if (x12.Count96() == 3) {// must have 2 locked digits  
			for (int i = 0, bit = 1; i < 9; i++, bit <<= 1) {
				if (!(bit & vcmore)) continue;
				BF128 wd = rdm[i] & units3xBM[iu];
				if ((wd - x12).isNotEmpty()) vcmore ^= bit;	
			}
			if (_popcnt32(vcmore) != 2) return 0;
			vcum = vcmore;
		}		
		else if (x12.Count96() ==2) {// try  one cell extra digits 
			BF128 w; w.SetAll_0();
			for (int i = 0, bit = 1; i < 9; i++, bit <<= 1) {
				if (!(bit & vcmore)) continue;
				BF128 wd = rdm[i] & units3xBM[iu];
				register int na = wd.Count96(), nb = (wd & x12).Count96();
				if ((na > 3) || ((na - nb) > 1)) {
					vcmore ^= bit;	continue;	}
				w |= (wd - x12); // extra cell				
			}
			register int nna = w.Count96(), nnb = _popcnt32(vcmore);
			if (nna>2 || nnb<2) return 0;
			if(nna==1 && nnb!=2) return 0;
			if (nna == 2 && nnb != 3) return 0;// same as 1+2
			x12 |= w;// all cells to clean
			vcum = vcmore;
		}
		else return 0; // 1 would be hiddeb pair		
		// clear if digits to clean in x12 now 3 cells 
		int digh4 = u2.digs | vcum;
		int iret = 0;
		for (int i = 0, bit = 1; i < 9; i++, bit <<= 1) {
			if (bit & digh4) continue;
			BF128 wd = rdm[i] & x12;
			if (wd.isNotEmpty()) {
				iret++;			psv->Clean(i,wd);		}
		}
		return iret;
	}
	int IelN3a(int iu2, int iu) {// pattern ac bc ur->abxy abxy
		U2& u2 = tu2[iu2];
		int alld = rcells[u2.c3] | rcells[u2.c4],
			more = alld & ~u2.digs,		nmore= _popcnt32(more);
		if (nmore != 2) return 0;
		//cout << "try nake triplet  2 more" << endl;
		BF128 wu = runs&units3xBM[iu];// unsolved in unit
		if (wu.Count96() < 5) return 0;// no possible effect
		BF128 wu_3 = ccm_3 & wu;//want max 3 digits in cell
		BF128 wubiv = wu & rcbiv;//want 2 biv
		int md1, md2;
		bitscanforward(md1, more); bitscanreverse(md2, more);
		BF128 w12 = (rdm[md1] | rdm[md2]) & wu;
		w12.Clear_c(u2.c3); w12.Clear_c(u2.c4);// where can be triplet
		BF128 wbiv = (rdm[md1] | rdm[md2]) & wubiv;
		BF128 w_3 = (rdm[md1] | rdm[md2]) & wu_3;
		for (int i = 0, bit = 1; i < 9; i++, bit <<= 1) {
			if (bit & alld) continue;
			int dig3 = more | bit;
			BF128 wd = rdm[i] & units3xBM[iu],
				wd_3=wd&w_3,
				wdp = wd & wbiv;
			if (wd_3.Count96() >= 2) {
				BF128 x = wd_3;
				int cell;
				while ((cell = x.getFirstCell()) >= 0) {
					x.Clear_c(cell);
					if(rcells[cell]& ~dig3)wd_3.Clear_c(cell);
				}
				if (wd_3.Count96() != 2) {
					//cout << " false triplet" << endl;	
					continue;	}
				BF128 wclean = wu - wd_3; wclean.Clear_c(u2.c3); wclean.Clear_c(u2.c4);
				if (wclean.isEmpty()) return 0;
				cout <<Char9out(dig3) << " we have now a nake triplet " << endl;
				BF128 wa = rdm[md1] & wclean, wb = rdm[md2] & wclean,
					wc = rdm[i] & wclean;
				int iret = 0;
				if (wa.isNotEmpty()) {iret++; psv->Clean(md1, wa);}
				if (wb.isNotEmpty()) { iret++; psv->Clean(md2, wb); }
				if (wc.isNotEmpty()) { iret++; psv->Clean(i, wc); }
				return iret;
			}
		}
		return 0;
	}
	int IelN3b(int iu2, int iu) {// pattern ac bc ur->abxy abxy
		U2& u2 = tu2[iu2];
		int alld = rcells[u2.c3] | rcells[u2.c4],
			more = alld & ~u2.digs, nmore = _popcnt32(more);
		if (nmore != 3) return 0;
		if(debug)cout <<Char9out(more)<< " try nake triplet 3 more" << endl;
		BF128 wu = runs & units3xBM[iu],w=wu;// all cells
		if (debug)cout << w.String3X(ws) << " active cells" << endl;
		for (int id = 0, bit = 1; id < 9; id++, bit <<= 1) {
			if (!(bit & more)) 	w -= rdm[id];
		}
		if (w.Count96() != 2) return 0;
		w.Set_c(u2.c3); w.Set_c(u2.c4);// locked cells
		wu -= w;// cells to clean if 
		int iret = 0;
		for (int i = 0, bit = 1; i < 9; i++, bit <<= 1) {
			if(! (bit & more)) continue;
			BF128 wd = rdm[i] & wu;
			if (wd.isNotEmpty()) {
				psv->Clean(i, wd); iret++;	}
		}
		return iret;
	}
	int IelN3(int iu2, int iu) {//2 more plus 2
		U2& u2 = tu2[iu2];
		int alld = rcells[u2.c3] | rcells[u2.c4],
			more = alld & ~u2.digs, nmore = _popcnt32(more),
			iret = 0;
		if (nmore > 3) return 0;
		//cout << "ieln3 try nake triplet  iu " << iu + 1 << endl;
		BF128 wu = runs & units3xBM[iu];// unsolved in unit
		if (wu.Count96() < 6) return 0;// no possible effect
		// clear all >4 or having digs
		BF128 x = wu, y;		y.SetAll_0();
		while ((cell = x.getFirstCell()) >= 0) {
			x.Clear_c(cell);
			register int v = rcells[cell];
			if (v & u2.digs)	wu.Clear_c(cell);
			if (_popcnt32(v & ~more) > 2)wu.Clear_c(cell);
			if (v & more) y.Set_c(cell);
		}
		int cell1, cell2;
		BF128 x1 = wu;
		while ((cell1 = x1.getFirstCell()) >= 0) {
			x1.Clear_c(cell1);
			int v1 = rcells[cell1], cumv1 = v1 | more,
				nv1 = _popcnt32(cumv1);
			if (nv1 > 3) continue;
			BF128 x2 = x1;
			while ((cell2 = x2.getFirstCell()) >= 0) {
				x2.Clear_c(cell2);
				int v2 = rcells[cell2], cumv2 = v2 | cumv1,
					nv2 = _popcnt32(cumv2);
				if (nv2 > 3) continue;
				cout << " seen naked triplet " << cell1 + 1 << " " 
					<< cell2 + 1 << " "	 << endl;
				BF128 wc = runs & units3xBM[iu];
				wc.Clear_c(cell1); wc.Clear_c(cell2); 
				wc.Clear_c(u2.c3); wc.Clear_c(u2.c4);
				for (int i = 0, bit = 1; i < 9; i++, bit <<= 1) {
					if (!(bit & cumv2)) continue;
					BF128 wcd = rdm[i] & wc;
					if (wcd.isNotEmpty()) { iret++; psv->Clean(i, wcd); }
				}
			}
		}
		return iret;
	}
	int IelN4(int iu2, int iu) {//2 more plus 2
		U2& u2 = tu2[iu2];
		int alld = rcells[u2.c3] | rcells[u2.c4],
			more = alld & ~u2.digs, nmore = _popcnt32(more),
			iret = 0;
		if (nmore >4) return 0;
		//cout << "ieln4 try nake quad  2 more iu "<<iu+1 << endl;
		BF128 wu = runs & units3xBM[iu],
			wur=wu;// unsolved in unit
		if (wu.Count96() < 6) return 0;// no possible effect
		wur.Clear_c(u2.c3); wur.Clear_c(u2.c4);
		// clear all >4  including more
		BF128 x = wur,y=x;
		while ((cell = x.getFirstCell()) >= 0) {
			x.Clear_c(cell);
			register int v = rcells[cell]|more;
			if (_popcnt32(v) > 4)y.Clear_c(cell);
		}
		//cout << y.String3X(ws) << " y to see for quad" << endl;
		int cell1, cell2, cell3;
		while ((cell1 = y.getFirstCell()) >= 0) {
			y.Clear_c(cell1);
			int v1 = rcells[cell1],cumv1=v1|more;
			if (_popcnt32(cumv1) > 4) continue;
			BF128 x2 = y;
			while ((cell2 = x2.getFirstCell()) >= 0) {
				x2.Clear_c(cell2);
				int v2 = rcells[cell2], cumv2 = v2 | cumv1;
				if (_popcnt32(cumv2) > 4) continue;
				BF128 x3 = x2;
				while ((cell3 = x3.getFirstCell()) >= 0) {
					x3.Clear_c(cell3);
					int v3 = rcells[cell3], cumv3 = v3 | cumv2;
					if (_popcnt32(cumv3) != 4) continue;
					//cout << " seen naked quad " << cell1 + 1 << " " << cell2 + 1 << " "
					//	<< cell3 + 1 << endl;
					BF128 wc = wu; // for digits of the UR
					wc.Clear_c(cell1); wc.Clear_c(cell2); wc.Clear_c(cell3);
					BF128 wcr =wc &  wur;// for other digits 
					for (int i = 0, bit = 1; i < 9; i++, bit <<= 1) {
						if (!(bit & cumv3)) continue;
						BF128 wcd = rdm[i] & wcr;
						if((bit & u2.digs))wcd = rdm[i] & wc;
						if (wcd.isNotEmpty()) { iret++; psv->Clean(i, wcd); }
					}
				}
			}
		}
		return iret;

	}
	
}ur_ul;

void UR_UL::Init_urul() {
	ps = &serate;	pso = &solve;	psv = &pso->sv81w;
	rdm = svr.dm;
	ws[81] = 0; debug = 0;// for debugging 
}
//==========digit pair  recursive search for UL 9 boxes to consider
void  UR_UL::InitDoElSearch(UR_ULD&u ,int nbox) {
	// init spot 0
	digs = u.digs;
	UL_SPOT& s = ulsp[0];
	memset(ulsp, 0, sizeof ulsp[0]);
	s.zb = u.my2d; s.z2 = u.x2d - u.my2d;
	s.boxes_to_see = u.ba;
	s.nboxtotake = nbox;
	s.rcells = rcells;
	s.digs = u.digs;
	s.debug = debug;
}

//=============== ULSPOT using UR_UL
void UL_SPOT::GoLastRC() {
	char ws[82]; ws[81] = 0;
	Addc(c1); Addc(c2);
	BF128 wadd = zass - ur_ul.rcbiv;
	int nadd = wadd.Count96();
	if (nadd == 1) { ThisIsURUL1(); return; }
	if (z_one_valid.isNotEmpty()) {
		ThisIsURUL2(); return;
	}
	if (nadd == 2) {
		ThisIsURUL3(); // not same unit or bi value
		if (!ur_ul.active_ul)StoreIt();
	}
}
void UL_SPOT::GetLastRC() {
	char ws[82]; ws[81] = 0;
	int itrcb = ibs / 3;  // row in band, col in stack
	BF128 x = (z2 | zb) & units3xBM[ibox],
		x1 = x & units3xBM[unit1], x2 = x & units3xBM[unit2];
	if (x1.isEmpty() || x2.isEmpty()) return;
	while ((c1 = x1.getFirstCell()) >= 0) {
		x1.Clear_c(c1);
		BF128 y = x2;
		while ((c2 = y.getFirstCell()) >= 0) {
			y.Clear_c(c2);
			if (c1 == c2) continue;
			int itrcb = ibs / 3;  // row in band, col in stack
			int crossx = tcellsrcb3[c1][1 - itrcb];
			int crossy = tcellsrcb3[c2][1 - itrcb];
			if (crossx != crossy) continue;;
			// can be more than one, finish in next spot
			UL_SPOT* sn = this + 1;
			*sn = *this;
			sn->GoLastRC();
		}
	}
}

void UL_SPOT::GoLastUL4() {	//call for last  missing  rows 2 cols
	int r1, r2, l1, l2,box;
	{
		register int x = rc18;
		bitscanforward(r1, x); x ^= 1 << r1;
		bitscanforward(r2, x); x ^= 1 << r2;
		bitscanforward(l1, x); x ^= 1 << l1;
		bitscanforward(l2, x);
	}
	box = b9per_BS[r1 / 3] & b9per_BS[l1 / 3];
	if (!(box & boxes_to_see)) return;
	// can be 2 cross in 4 cells
	bitscanforward(ib9, box); ibox = ib9 + 18;
	BF128 w = (z2 | zb) & units3xBM[ibox],
		rr1 = w & units3xBM[r1], ll1 = w & units3xBM[l1],
		rr2 = w & units3xBM[r2], ll2 = w & units3xBM[l2];
	c1 = (rr1 & ll1).getFirstCell(); c2 = (rr2 & ll2).getFirstCell();
	if (c1 >= 0 && c2 >= 0)GoLastRC();
	c1 = (rr1 & ll2).getFirstCell(); c2 = (rr2 & ll1).getFirstCell();
	if (c1 >= 0 && c2 >= 0)GoLastRC();
}


void UL_SPOT::ThisIsUL() {
	char ws[82]; ws[81] = 0;
	BF128 wadd = zass - ur_ul.rcbiv;
	int nadd = wadd.Count96();
	if (nadd == 1) { ThisIsURUL1(); return; }
	if (z_one_valid.isNotEmpty()) {
		ThisIsURUL2(); return;
	}
	if (nadd == 2) {
		ThisIsURUL3(); // not same unit or bi value
		if (!ur_ul.active_ul)StoreIt();
	}
	
}

void UL_SPOT::ThisIsURUL1() {// one cell
	char ws[82]; ws[81] = 0;
	BF128 wadd = zass - ur_ul.rcbiv; // cell to clean
	int cell = wadd.getFirstCell();
	ur_ul.psv->CleanCell(cell, ur_ul.uld.digs);
	ur_ul.active_ul++;
}
void UL_SPOT::ThisIsURUL2() {// one digit active
	char ws[82]; ws[81] = 0;
	int d_one; bitscanforward(d_one, moreone);
	BF128 w= z_one_valid & ur_ul.rdm[d_one];
	if (w.isNotEmpty()) {
		ur_ul.psv->Clean(d_one, w);
		ur_ul.active_ul++;
		return ;
	}
	return ;
}
void UL_SPOT::ThisIsURUL3() {// one digit bi value
	char ws[82]; ws[81] = 0;
	BF128 wadd = zass - ur_ul.rcbiv; // cells to clean
	int c3 = wadd.getFirstCell(), c4 = wadd.getLastCell();
	BF128 w1 = cell_z3x[c3]; 
	if (w1.Off_c(c4)) return ;// not one unit
	// look for digit locked
	int rcb = tcellsrcb[c3] & tcellsrcb[c4], iu,
		d1 = ur_ul.uld.d1, d2 = ur_ul.uld.d2, digs = ur_ul.uld.digs;
	BF128 wul; wul.SetAll_0();
	wul.Set_c(c3); wul.Set_c(c4);
	int v = 0;
	//cout << Char27out(rcb) << " rcb" << endl;
	while (rcb) {// r c b
		bitscanforward(iu, rcb);
		rcb ^= 1 << iu;
		BF128 w1 = ur_ul.rdm[d1] & units3xBM[iu],
			w2 = ur_ul.rdm[d2] & units3xBM[iu];
		if (w1.Count96() == 2) {
			ur_ul.psv->Clean(d2, wul); v++;
		}
		if (w2.Count96() == 2) {
			ur_ul.psv->Clean(d1, wul); v++;
		}
	}
	if (v)ur_ul.active_ul++;	
}
void UL_SPOT::StoreIt() {
	if (!(ur_ul.NewURUL(zass))) return;
	BF128 w = zass - ur_ul.rcbiv;
	ur_ul.tu2[ur_ul.ntu2++].Add(zass, 
		w.getFirstCell(), w.getLastCell(),
		ur_ul.uld.digs, ur_ul.drstore);
}
