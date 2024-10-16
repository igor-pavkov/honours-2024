static void codeDistinct(
  Parse *pParse,     /* Parsing and code generating context */
  int iTab,          /* A sorting index used to test for distinctness */
  int addrRepeat,    /* Jump to here if not distinct */
  int N,             /* Number of elements */
  int iMem           /* First element */
){
  Vdbe *v;
  int r1;

  v = pParse->pVdbe;
  r1 = sqlite3GetTempReg(pParse);
  sqlite3VdbeAddOp4Int(v, OP_Found, iTab, addrRepeat, iMem, N); VdbeCoverage(v);
  sqlite3VdbeAddOp3(v, OP_MakeRecord, iMem, N, r1);
  sqlite3VdbeAddOp4Int(v, OP_IdxInsert, iTab, r1, iMem, N);
  sqlite3VdbeChangeP5(v, OPFLAG_USESEEKRESULT);
  sqlite3ReleaseTempReg(pParse, r1);
}