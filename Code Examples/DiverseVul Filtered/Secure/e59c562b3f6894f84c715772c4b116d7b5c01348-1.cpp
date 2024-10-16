static int multiSelectOrderBy(
  Parse *pParse,        /* Parsing context */
  Select *p,            /* The right-most of SELECTs to be coded */
  SelectDest *pDest     /* What to do with query results */
){
  int i, j;             /* Loop counters */
  Select *pPrior;       /* Another SELECT immediately to our left */
  Vdbe *v;              /* Generate code to this VDBE */
  SelectDest destA;     /* Destination for coroutine A */
  SelectDest destB;     /* Destination for coroutine B */
  int regAddrA;         /* Address register for select-A coroutine */
  int regAddrB;         /* Address register for select-B coroutine */
  int addrSelectA;      /* Address of the select-A coroutine */
  int addrSelectB;      /* Address of the select-B coroutine */
  int regOutA;          /* Address register for the output-A subroutine */
  int regOutB;          /* Address register for the output-B subroutine */
  int addrOutA;         /* Address of the output-A subroutine */
  int addrOutB = 0;     /* Address of the output-B subroutine */
  int addrEofA;         /* Address of the select-A-exhausted subroutine */
  int addrEofA_noB;     /* Alternate addrEofA if B is uninitialized */
  int addrEofB;         /* Address of the select-B-exhausted subroutine */
  int addrAltB;         /* Address of the A<B subroutine */
  int addrAeqB;         /* Address of the A==B subroutine */
  int addrAgtB;         /* Address of the A>B subroutine */
  int regLimitA;        /* Limit register for select-A */
  int regLimitB;        /* Limit register for select-A */
  int regPrev;          /* A range of registers to hold previous output */
  int savedLimit;       /* Saved value of p->iLimit */
  int savedOffset;      /* Saved value of p->iOffset */
  int labelCmpr;        /* Label for the start of the merge algorithm */
  int labelEnd;         /* Label for the end of the overall SELECT stmt */
  int addr1;            /* Jump instructions that get retargetted */
  int op;               /* One of TK_ALL, TK_UNION, TK_EXCEPT, TK_INTERSECT */
  KeyInfo *pKeyDup = 0; /* Comparison information for duplicate removal */
  KeyInfo *pKeyMerge;   /* Comparison information for merging rows */
  sqlite3 *db;          /* Database connection */
  ExprList *pOrderBy;   /* The ORDER BY clause */
  int nOrderBy;         /* Number of terms in the ORDER BY clause */
  int *aPermute;        /* Mapping from ORDER BY terms to result set columns */

  assert( p->pOrderBy!=0 );
  assert( pKeyDup==0 ); /* "Managed" code needs this.  Ticket #3382. */
  db = pParse->db;
  v = pParse->pVdbe;
  assert( v!=0 );       /* Already thrown the error if VDBE alloc failed */
  labelEnd = sqlite3VdbeMakeLabel(pParse);
  labelCmpr = sqlite3VdbeMakeLabel(pParse);


  /* Patch up the ORDER BY clause
  */
  op = p->op;  
  pPrior = p->pPrior;
  assert( pPrior->pOrderBy==0 );
  pOrderBy = p->pOrderBy;
  assert( pOrderBy );
  nOrderBy = pOrderBy->nExpr;

  /* For operators other than UNION ALL we have to make sure that
  ** the ORDER BY clause covers every term of the result set.  Add
  ** terms to the ORDER BY clause as necessary.
  */
  if( op!=TK_ALL ){
    for(i=1; db->mallocFailed==0 && i<=p->pEList->nExpr; i++){
      struct ExprList_item *pItem;
      for(j=0, pItem=pOrderBy->a; j<nOrderBy; j++, pItem++){
        assert( pItem->u.x.iOrderByCol>0 );
        if( pItem->u.x.iOrderByCol==i ) break;
      }
      if( j==nOrderBy ){
        Expr *pNew = sqlite3Expr(db, TK_INTEGER, 0);
        if( pNew==0 ) return SQLITE_NOMEM_BKPT;
        pNew->flags |= EP_IntValue;
        pNew->u.iValue = i;
        p->pOrderBy = pOrderBy = sqlite3ExprListAppend(pParse, pOrderBy, pNew);
        if( pOrderBy ) pOrderBy->a[nOrderBy++].u.x.iOrderByCol = (u16)i;
      }
    }
  }

  /* Compute the comparison permutation and keyinfo that is used with
  ** the permutation used to determine if the next
  ** row of results comes from selectA or selectB.  Also add explicit
  ** collations to the ORDER BY clause terms so that when the subqueries
  ** to the right and the left are evaluated, they use the correct
  ** collation.
  */
  aPermute = sqlite3DbMallocRawNN(db, sizeof(int)*(nOrderBy + 1));
  if( aPermute ){
    struct ExprList_item *pItem;
    aPermute[0] = nOrderBy;
    for(i=1, pItem=pOrderBy->a; i<=nOrderBy; i++, pItem++){
      assert( pItem->u.x.iOrderByCol>0 );
      assert( pItem->u.x.iOrderByCol<=p->pEList->nExpr );
      aPermute[i] = pItem->u.x.iOrderByCol - 1;
    }
    pKeyMerge = multiSelectOrderByKeyInfo(pParse, p, 1);
  }else{
    pKeyMerge = 0;
  }

  /* Reattach the ORDER BY clause to the query.
  */
  p->pOrderBy = pOrderBy;
  pPrior->pOrderBy = sqlite3ExprListDup(pParse->db, pOrderBy, 0);

  /* Allocate a range of temporary registers and the KeyInfo needed
  ** for the logic that removes duplicate result rows when the
  ** operator is UNION, EXCEPT, or INTERSECT (but not UNION ALL).
  */
  if( op==TK_ALL ){
    regPrev = 0;
  }else{
    int nExpr = p->pEList->nExpr;
    assert( nOrderBy>=nExpr || db->mallocFailed );
    regPrev = pParse->nMem+1;
    pParse->nMem += nExpr+1;
    sqlite3VdbeAddOp2(v, OP_Integer, 0, regPrev);
    pKeyDup = sqlite3KeyInfoAlloc(db, nExpr, 1);
    if( pKeyDup ){
      assert( sqlite3KeyInfoIsWriteable(pKeyDup) );
      for(i=0; i<nExpr; i++){
        pKeyDup->aColl[i] = multiSelectCollSeq(pParse, p, i);
        pKeyDup->aSortFlags[i] = 0;
      }
    }
  }
 
  /* Separate the left and the right query from one another
  */
  p->pPrior = 0;
  pPrior->pNext = 0;
  sqlite3ResolveOrderGroupBy(pParse, p, p->pOrderBy, "ORDER");
  if( pPrior->pPrior==0 ){
    sqlite3ResolveOrderGroupBy(pParse, pPrior, pPrior->pOrderBy, "ORDER");
  }

  /* Compute the limit registers */
  computeLimitRegisters(pParse, p, labelEnd);
  if( p->iLimit && op==TK_ALL ){
    regLimitA = ++pParse->nMem;
    regLimitB = ++pParse->nMem;
    sqlite3VdbeAddOp2(v, OP_Copy, p->iOffset ? p->iOffset+1 : p->iLimit,
                                  regLimitA);
    sqlite3VdbeAddOp2(v, OP_Copy, regLimitA, regLimitB);
  }else{
    regLimitA = regLimitB = 0;
  }
  sqlite3ExprDelete(db, p->pLimit);
  p->pLimit = 0;

  regAddrA = ++pParse->nMem;
  regAddrB = ++pParse->nMem;
  regOutA = ++pParse->nMem;
  regOutB = ++pParse->nMem;
  sqlite3SelectDestInit(&destA, SRT_Coroutine, regAddrA);
  sqlite3SelectDestInit(&destB, SRT_Coroutine, regAddrB);

  ExplainQueryPlan((pParse, 1, "MERGE (%s)", selectOpName(p->op)));

  /* Generate a coroutine to evaluate the SELECT statement to the
  ** left of the compound operator - the "A" select.
  */
  addrSelectA = sqlite3VdbeCurrentAddr(v) + 1;
  addr1 = sqlite3VdbeAddOp3(v, OP_InitCoroutine, regAddrA, 0, addrSelectA);
  VdbeComment((v, "left SELECT"));
  pPrior->iLimit = regLimitA;
  ExplainQueryPlan((pParse, 1, "LEFT"));
  sqlite3Select(pParse, pPrior, &destA);
  sqlite3VdbeEndCoroutine(v, regAddrA);
  sqlite3VdbeJumpHere(v, addr1);

  /* Generate a coroutine to evaluate the SELECT statement on 
  ** the right - the "B" select
  */
  addrSelectB = sqlite3VdbeCurrentAddr(v) + 1;
  addr1 = sqlite3VdbeAddOp3(v, OP_InitCoroutine, regAddrB, 0, addrSelectB);
  VdbeComment((v, "right SELECT"));
  savedLimit = p->iLimit;
  savedOffset = p->iOffset;
  p->iLimit = regLimitB;
  p->iOffset = 0;  
  ExplainQueryPlan((pParse, 1, "RIGHT"));
  sqlite3Select(pParse, p, &destB);
  p->iLimit = savedLimit;
  p->iOffset = savedOffset;
  sqlite3VdbeEndCoroutine(v, regAddrB);

  /* Generate a subroutine that outputs the current row of the A
  ** select as the next output row of the compound select.
  */
  VdbeNoopComment((v, "Output routine for A"));
  addrOutA = generateOutputSubroutine(pParse,
                 p, &destA, pDest, regOutA,
                 regPrev, pKeyDup, labelEnd);
  
  /* Generate a subroutine that outputs the current row of the B
  ** select as the next output row of the compound select.
  */
  if( op==TK_ALL || op==TK_UNION ){
    VdbeNoopComment((v, "Output routine for B"));
    addrOutB = generateOutputSubroutine(pParse,
                 p, &destB, pDest, regOutB,
                 regPrev, pKeyDup, labelEnd);
  }
  sqlite3KeyInfoUnref(pKeyDup);

  /* Generate a subroutine to run when the results from select A
  ** are exhausted and only data in select B remains.
  */
  if( op==TK_EXCEPT || op==TK_INTERSECT ){
    addrEofA_noB = addrEofA = labelEnd;
  }else{  
    VdbeNoopComment((v, "eof-A subroutine"));
    addrEofA = sqlite3VdbeAddOp2(v, OP_Gosub, regOutB, addrOutB);
    addrEofA_noB = sqlite3VdbeAddOp2(v, OP_Yield, regAddrB, labelEnd);
                                     VdbeCoverage(v);
    sqlite3VdbeGoto(v, addrEofA);
    p->nSelectRow = sqlite3LogEstAdd(p->nSelectRow, pPrior->nSelectRow);
  }

  /* Generate a subroutine to run when the results from select B
  ** are exhausted and only data in select A remains.
  */
  if( op==TK_INTERSECT ){
    addrEofB = addrEofA;
    if( p->nSelectRow > pPrior->nSelectRow ) p->nSelectRow = pPrior->nSelectRow;
  }else{  
    VdbeNoopComment((v, "eof-B subroutine"));
    addrEofB = sqlite3VdbeAddOp2(v, OP_Gosub, regOutA, addrOutA);
    sqlite3VdbeAddOp2(v, OP_Yield, regAddrA, labelEnd); VdbeCoverage(v);
    sqlite3VdbeGoto(v, addrEofB);
  }

  /* Generate code to handle the case of A<B
  */
  VdbeNoopComment((v, "A-lt-B subroutine"));
  addrAltB = sqlite3VdbeAddOp2(v, OP_Gosub, regOutA, addrOutA);
  sqlite3VdbeAddOp2(v, OP_Yield, regAddrA, addrEofA); VdbeCoverage(v);
  sqlite3VdbeGoto(v, labelCmpr);

  /* Generate code to handle the case of A==B
  */
  if( op==TK_ALL ){
    addrAeqB = addrAltB;
  }else if( op==TK_INTERSECT ){
    addrAeqB = addrAltB;
    addrAltB++;
  }else{
    VdbeNoopComment((v, "A-eq-B subroutine"));
    addrAeqB =
    sqlite3VdbeAddOp2(v, OP_Yield, regAddrA, addrEofA); VdbeCoverage(v);
    sqlite3VdbeGoto(v, labelCmpr);
  }

  /* Generate code to handle the case of A>B
  */
  VdbeNoopComment((v, "A-gt-B subroutine"));
  addrAgtB = sqlite3VdbeCurrentAddr(v);
  if( op==TK_ALL || op==TK_UNION ){
    sqlite3VdbeAddOp2(v, OP_Gosub, regOutB, addrOutB);
  }
  sqlite3VdbeAddOp2(v, OP_Yield, regAddrB, addrEofB); VdbeCoverage(v);
  sqlite3VdbeGoto(v, labelCmpr);

  /* This code runs once to initialize everything.
  */
  sqlite3VdbeJumpHere(v, addr1);
  sqlite3VdbeAddOp2(v, OP_Yield, regAddrA, addrEofA_noB); VdbeCoverage(v);
  sqlite3VdbeAddOp2(v, OP_Yield, regAddrB, addrEofB); VdbeCoverage(v);

  /* Implement the main merge loop
  */
  sqlite3VdbeResolveLabel(v, labelCmpr);
  sqlite3VdbeAddOp4(v, OP_Permutation, 0, 0, 0, (char*)aPermute, P4_INTARRAY);
  sqlite3VdbeAddOp4(v, OP_Compare, destA.iSdst, destB.iSdst, nOrderBy,
                         (char*)pKeyMerge, P4_KEYINFO);
  sqlite3VdbeChangeP5(v, OPFLAG_PERMUTE);
  sqlite3VdbeAddOp3(v, OP_Jump, addrAltB, addrAeqB, addrAgtB); VdbeCoverage(v);

  /* Jump to the this point in order to terminate the query.
  */
  sqlite3VdbeResolveLabel(v, labelEnd);

  /* Reassembly the compound query so that it will be freed correctly
  ** by the calling function */
  if( p->pPrior ){
    sqlite3SelectDelete(db, p->pPrior);
  }
  p->pPrior = pPrior;
  pPrior->pNext = p;

  /*** TBD:  Insert subroutine calls to close cursors on incomplete
  **** subqueries ****/
  ExplainQueryPlanPop(pParse);
  return pParse->nErr!=0;
}