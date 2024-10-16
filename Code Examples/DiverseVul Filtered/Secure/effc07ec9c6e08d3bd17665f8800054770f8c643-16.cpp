static SQLITE_NOINLINE void sqlite3ConstructBloomFilter(
  WhereInfo *pWInfo,    /* The WHERE clause */
  int iLevel,           /* Index in pWInfo->a[] that is pLevel */
  WhereLevel *pLevel,   /* Make a Bloom filter for this FROM term */
  Bitmask notReady      /* Loops that are not ready */
){
  int addrOnce;                        /* Address of opening OP_Once */
  int addrTop;                         /* Address of OP_Rewind */
  int addrCont;                        /* Jump here to skip a row */
  const WhereTerm *pTerm;              /* For looping over WHERE clause terms */
  const WhereTerm *pWCEnd;             /* Last WHERE clause term */
  Parse *pParse = pWInfo->pParse;      /* Parsing context */
  Vdbe *v = pParse->pVdbe;             /* VDBE under construction */
  WhereLoop *pLoop = pLevel->pWLoop;   /* The loop being coded */
  int iCur;                            /* Cursor for table getting the filter */

  assert( pLoop!=0 );
  assert( v!=0 );
  assert( pLoop->wsFlags & WHERE_BLOOMFILTER );

  addrOnce = sqlite3VdbeAddOp0(v, OP_Once); VdbeCoverage(v);
  do{
    const SrcItem *pItem;
    const Table *pTab;
    u64 sz;
    sqlite3WhereExplainBloomFilter(pParse, pWInfo, pLevel);
    addrCont = sqlite3VdbeMakeLabel(pParse);
    iCur = pLevel->iTabCur;
    pLevel->regFilter = ++pParse->nMem;

    /* The Bloom filter is a Blob held in a register.  Initialize it
    ** to zero-filled blob of at least 80K bits, but maybe more if the
    ** estimated size of the table is larger.  We could actually
    ** measure the size of the table at run-time using OP_Count with
    ** P3==1 and use that value to initialize the blob.  But that makes
    ** testing complicated.  By basing the blob size on the value in the
    ** sqlite_stat1 table, testing is much easier.
    */
    pItem = &pWInfo->pTabList->a[pLevel->iFrom];
    assert( pItem!=0 );
    pTab = pItem->pTab;
    assert( pTab!=0 );
    sz = sqlite3LogEstToInt(pTab->nRowLogEst);
    if( sz<10000 ){
      sz = 10000;
    }else if( sz>10000000 ){
      sz = 10000000;
    }
    sqlite3VdbeAddOp2(v, OP_Blob, (int)sz, pLevel->regFilter);

    addrTop = sqlite3VdbeAddOp1(v, OP_Rewind, iCur); VdbeCoverage(v);
    pWCEnd = &pWInfo->sWC.a[pWInfo->sWC.nTerm];
    for(pTerm=pWInfo->sWC.a; pTerm<pWCEnd; pTerm++){
      Expr *pExpr = pTerm->pExpr;
      if( (pTerm->wtFlags & TERM_VIRTUAL)==0
       && sqlite3ExprIsTableConstraint(pExpr, pItem)
      ){
        sqlite3ExprIfFalse(pParse, pTerm->pExpr, addrCont, SQLITE_JUMPIFNULL);
      }
    }
    if( pLoop->wsFlags & WHERE_IPK ){
      int r1 = sqlite3GetTempReg(pParse);
      sqlite3VdbeAddOp2(v, OP_Rowid, iCur, r1);
      sqlite3VdbeAddOp4Int(v, OP_FilterAdd, pLevel->regFilter, 0, r1, 1);
      sqlite3ReleaseTempReg(pParse, r1);
    }else{
      Index *pIdx = pLoop->u.btree.pIndex;
      int n = pLoop->u.btree.nEq;
      int r1 = sqlite3GetTempRange(pParse, n);
      int jj;
      for(jj=0; jj<n; jj++){
        int iCol = pIdx->aiColumn[jj];
        assert( pIdx->pTable==pItem->pTab );
        sqlite3ExprCodeGetColumnOfTable(v, pIdx->pTable, iCur, iCol,r1+jj);
      }
      sqlite3VdbeAddOp4Int(v, OP_FilterAdd, pLevel->regFilter, 0, r1, n);
      sqlite3ReleaseTempRange(pParse, r1, n);
    }
    sqlite3VdbeResolveLabel(v, addrCont);
    sqlite3VdbeAddOp2(v, OP_Next, pLevel->iTabCur, addrTop+1);
    VdbeCoverage(v);
    sqlite3VdbeJumpHere(v, addrTop);
    pLoop->wsFlags &= ~WHERE_BLOOMFILTER;
    if( OptimizationDisabled(pParse->db, SQLITE_BloomPulldown) ) break;
    while( ++iLevel < pWInfo->nLevel ){
      const SrcItem *pTabItem;
      pLevel = &pWInfo->a[iLevel];
      pTabItem = &pWInfo->pTabList->a[pLevel->iFrom];
      if( pTabItem->fg.jointype & (JT_LEFT|JT_LTORJ) ) continue;
      pLoop = pLevel->pWLoop;
      if( NEVER(pLoop==0) ) continue;
      if( pLoop->prereq & notReady ) continue;
      if( (pLoop->wsFlags & (WHERE_BLOOMFILTER|WHERE_COLUMN_IN))
                 ==WHERE_BLOOMFILTER
      ){
        /* This is a candidate for bloom-filter pull-down (early evaluation).
        ** The test that WHERE_COLUMN_IN is omitted is important, as we are
        ** not able to do early evaluation of bloom filters that make use of
        ** the IN operator */
        break;
      }
    }
  }while( iLevel < pWInfo->nLevel );
  sqlite3VdbeJumpHere(v, addrOnce);
}