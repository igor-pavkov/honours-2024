static int whereRangeSkipScanEst(
  Parse *pParse,       /* Parsing & code generating context */
  WhereTerm *pLower,   /* Lower bound on the range. ex: "x>123" Might be NULL */
  WhereTerm *pUpper,   /* Upper bound on the range. ex: "x<455" Might be NULL */
  WhereLoop *pLoop,    /* Update the .nOut value of this loop */
  int *pbDone          /* Set to true if at least one expr. value extracted */
){
  Index *p = pLoop->u.btree.pIndex;
  int nEq = pLoop->u.btree.nEq;
  sqlite3 *db = pParse->db;
  int nLower = -1;
  int nUpper = p->nSample+1;
  int rc = SQLITE_OK;
  u8 aff = sqlite3IndexColumnAffinity(db, p, nEq);
  CollSeq *pColl;
  
  sqlite3_value *p1 = 0;          /* Value extracted from pLower */
  sqlite3_value *p2 = 0;          /* Value extracted from pUpper */
  sqlite3_value *pVal = 0;        /* Value extracted from record */

  pColl = sqlite3LocateCollSeq(pParse, p->azColl[nEq]);
  if( pLower ){
    rc = sqlite3Stat4ValueFromExpr(pParse, pLower->pExpr->pRight, aff, &p1);
    nLower = 0;
  }
  if( pUpper && rc==SQLITE_OK ){
    rc = sqlite3Stat4ValueFromExpr(pParse, pUpper->pExpr->pRight, aff, &p2);
    nUpper = p2 ? 0 : p->nSample;
  }

  if( p1 || p2 ){
    int i;
    int nDiff;
    for(i=0; rc==SQLITE_OK && i<p->nSample; i++){
      rc = sqlite3Stat4Column(db, p->aSample[i].p, p->aSample[i].n, nEq, &pVal);
      if( rc==SQLITE_OK && p1 ){
        int res = sqlite3MemCompare(p1, pVal, pColl);
        if( res>=0 ) nLower++;
      }
      if( rc==SQLITE_OK && p2 ){
        int res = sqlite3MemCompare(p2, pVal, pColl);
        if( res>=0 ) nUpper++;
      }
    }
    nDiff = (nUpper - nLower);
    if( nDiff<=0 ) nDiff = 1;

    /* If there is both an upper and lower bound specified, and the 
    ** comparisons indicate that they are close together, use the fallback
    ** method (assume that the scan visits 1/64 of the rows) for estimating
    ** the number of rows visited. Otherwise, estimate the number of rows
    ** using the method described in the header comment for this function. */
    if( nDiff!=1 || pUpper==0 || pLower==0 ){
      int nAdjust = (sqlite3LogEst(p->nSample) - sqlite3LogEst(nDiff));
      pLoop->nOut -= nAdjust;
      *pbDone = 1;
      WHERETRACE(0x10, ("range skip-scan regions: %u..%u  adjust=%d est=%d\n",
                           nLower, nUpper, nAdjust*-1, pLoop->nOut));
    }

  }else{
    assert( *pbDone==0 );
  }

  sqlite3ValueFree(p1);
  sqlite3ValueFree(p2);
  sqlite3ValueFree(pVal);

  return rc;
}