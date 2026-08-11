// working on mini rows mini columns
/*
for each digit a 54 2x bits status "is in mini row mini colums
a 27 units bits of active cells
unsolved unit digit 9 = 27

*/

void MINIS::InitSolPerDigit(int* sole) {
	memcpy(sol, sole, sizeof sol);
	memset(sold, 0, sizeof sold);
	for (int i = 0; i < 81; i++) sold[sole[i]].Set_c(i);
}
int MINIS::Build(SOLVE& o) {
	unsolved_cells = o.sv81w.unsolved_cells;
	int iret = 0;
	for (int id = 0; id < 9; id++) {// 9 digits
		BF128 dclean; dclean.SetAll_0();
		BF128 w = o.sv81w.dm[id] & unsolved_cells; // digit bits status
		//_________________ setup active mini rows cols
		{
			damr[id] = damc[id] = 0;
			for (int ib = 0; ib < 3; ib++) {// 3 bands
				register uint32_t r = w.bf.u32[ib];
				register uint32_t bbitsr = 0, bbitsc = 0;// set band  minirows
				for (uint64_t imr = 0; imr < 9; imr++)
					if (p3r_32[imr] & r)bbitsr |= bit9_32[imr];
				damr[id] |= (bbitsr << 9 * ib);
				for (uint64_t imc = 0; imc < 9; imc++)
					if (p3c_32[imc] & r)bbitsc |= bit9_32[imc];
				damc[id] |= (bbitsc << 9 * ib);
			}
		}
		//____ find and apply row box single mini rows
		{
			register uint32_t ram = damr[id], r1 = 0, r2 = 0;
			register uint32_t im = 0, bit = 1;
			for (; im < 27; im++, bit <<= 1) {// mini rows
				if (!(ram & bit)) continue;// inactive mini
				register uint32_t ir = im / 3, // row
					ib = im % 3 + 3 * (im / 9);//  box
				register uint32_t map = (1 << ir) | (1 << (ib + 9));
				r2 |= r1 & map; r1 |= map;
			}
			r1 &= ~r2;
			if (r1) {
				for (im = 0, bit = 1; im < 27; im++, bit <<= 1) {// mini rows
					if (!(ram & bit)) continue;// inactive mini
					register uint32_t ir = im / 3, // row
						ib = im % 3 + 3 * (im / 9);//  box
					register uint32_t bitr = (1 << ir), bitb(1 << (ib + 9));
					if ((r1 & bitr) && (r2 & bitb)) {
						BF128 wr = w & units3xBM[ir],
							wb = w & units3xBM[ib + 18];
						dclean |= (wb - wr);
						if (solve.opp & 2)
							cout << " clean box r b " << ir + 1 << " " << ib + 1 
							<< " d " << id+1 << endl;
					}
					if (r2 & bitr && r1 & bitb) {
						BF128 wr = w & units3xBM[ir],
							wb = w & units3xBM[ib + 18];
						dclean |= (wr - wb);
						if(solve.opp&2)
						cout << " clean row r b " << ir + 1 << " " << ib + 1
							<< " d " << id + 1 << endl;
					}
				}
			}
		}
		//____ find and apply cols box single mini colss
		{
			register uint32_t ram = damc[id], r1 = 0, r2 = 0;
			register uint32_t im = 0, bit = 1;
			for (; im < 27; im++, bit <<= 1) {// mini cols
				if (!(ram & bit)) continue;// inactive mini
				register uint32_t ic = im % 9, ib = im / 3;
				register uint32_t map = (1 << ic) | (1 << (ib + 9));
				r2 |= r1 & map; r1 |= map;
			}
			r1 &= ~r2;
			if (r1) {
				for (im = 0, bit = 1; im < 27; im++, bit <<= 1) {// mini rows
					if (!(ram & bit)) continue;// inactive mini
					register uint32_t ic = im % 9,ib = im / 3; 
					register uint32_t bitc = (1 << ic),	bitb(1 << (ib + 9));
					if ((r1 & bitc) && (r2 & bitb)) {
						BF128 wc = w & units3xBM[ic+9],
							wb = w & units3xBM[ib + 18];
						dclean |= (wb - wc);
						if (solve.opp & 2)
							cout << " clean box c b " << ic + 1 << " " << ib + 1 << " im= " << im << endl;
					}
					if (r2 & bitc && r1 & bitb) {
						BF128 wc = w & units3xBM[ic+9],
							wb = w & units3xBM[ib + 18];
						dclean |= (wc - wb);
						if (solve.opp & 2)
							cout << " clean col c b " << ic + 1 << " " << ib + 1 << " im= " << im << endl;
					}
				}
			}
		}
		if (dclean.isNotEmpty()) {
			iret++;
			if (solve.opp & 2) {
				cout << "clean digit " << id + 1 << endl;
				dclean.Print81();
			}
			solve.Clean(id, dclean);
		}
	}
	return iret;
}

void MINIS::DumpSolsPerDigit1() {
	cout << " dig sols per digit" << endl;
	char ws[82]; ws[81] = 0;
	cout << unsolved_cells.String3X(ws) << " unsolved cells"  << endl;
	//for (int i = 0; i < 9; i++)
	//	cout << sold[i].String3X(ws) << " " << i+1 << endl;
	cout << "dig nsolved" << endl;
	for (int i = 0; i < 9; i++)
		cout << (sold[i]& unsolved_cells).String3X(ws) << " " << i + 1 << endl;
}

void MINIS::Dump1() {
	cout << " minis status after build" << endl;
	cout << " rows digit mini status" << endl;
	for (int i = 0; i < 9; i++)
		cout << Char27out(damr[i]) << " " << i+1 << endl;
	cout << " colss digit mini status" << endl;
	for (int i = 0; i < 9; i++)
		cout << Char27out(damc[i]) << " " << i+1 << endl;
	return;
}

/*

byte miniline[54][3] =
{	{ 0, 1, 2},{ 3, 4, 5},{ 6, 7, 8},{ 9,10,11},{12,13,14},{15,16,17},{18,19,20},{21,22,23},{24,25,26},
	{27,28,29},{30,31,32},{33,34,35},{36,37,38},{39,40,41},{42,43,44},{45,46,47},{48,49,50},{51,52,53},
	{54,55,56},{57,58,59},{60,61,62},{63,64,65},{66,67,68},{69,70,71},{72,73,74},{75,76,77},{78,79,80},
	{ 0, 9,18},{27,36,45},{54,63,72},{ 1,10,19},{28,37,46},{55,64,73},{ 2,11,20},{29,38,47},{56,65,74},
	{ 3,12,21},{30,39,48},{57,66,75},{ 4,13,22},{31,40,49},{58,67,76},{ 5,14,23},{32,41,50},{59,68,77},
	{ 6,15,24},{33,42,51},{60,69,78},{ 7,16,25},{34,43,52},{61,70,79},{ 8,17,26},{35,44,53},{62,71,80}
};

byte box_miniline[54][3] =
{	{ 0, 1, 2},{ 9,10,11},{18,19,20},{ 0, 9,18},{ 1,10,19},{ 2,11,20},
	{ 3, 4, 5},{12,13,14},{21,22,23},{ 3,12,21},{ 4,13,22},{ 5,14,23},
	{ 6, 7, 8},{15,16,17},{24,25,26},{ 6,15,24},{ 7,16,25},{ 8,17,26},
	{27,28,29},{36,37,38},{45,46,47},{27,36,45},{28,37,46},{29,38,47},
	{30,31,32},{39,40,41},{48,49,50},{30,39,48},{31,40,49},{32,41,50},
	{33,34,35},{42,43,44},{51,52,53},{33,42,51},{34,43,52},{35,44,53},
	{54,55,56},{63,64,65},{72,73,74},{54,63,72},{55,64,73},{56,65,74},
	{57,58,59},{66,67,68},{75,76,77},{57,66,75},{58,67,76},{59,68,77},
	{60,61,62},{69,70,71},{78,79,80},{60,69,78},{61,70,79},{62,71,80}
};

uint64_t tbl_2x_cols[9] = { 0x4020100040201, 0x8040200080402, 0x10080400100804,
	0x20100800201008,0x40201000402010, 0x80402000804020,
	0x100804001008040, 0x201008002010080,0x402010004020100 };
uint64_t tbl_2x_stacks[3] = { 0x1c0e07001c0e07, 0xe0703800e07038, 0x70381c0070381c0 };
uint64_t tbl_2x_stacksn[3] = { 0xffe3f1f8ffe3f1f8, 0xff1f8fc7ff1f8fc7, 0xf8fc7e3ff8fc7e3f };
//0x40201001c0e07
*/