Select *sqlite3SelectNew(
  Parse *pParse,        /* Parsing context */
  ExprList *pEList,     /* which columns to include in the result */
  SrcList *pSrc,        /* the FROM clause -- which tables to scan */
  Expr *pWhere,         /* the WHERE clause */
  ExprList *pGroupBy,   /* the GROUP BY clause */
  Expr *pHaving,        /* the HAVING clause */
  ExprList *pOrderBy,   /* the ORDER BY clause */
  u32 selFlags,         /* Flag parameters, such as SF_Distinct */
  Expr *pLimit          /* LIMIT value.  NULL means not used */
){
  Select *pNew;
  Select standin;
  pNew = sqlite3DbMallocRawNN(pParse->db, sizeof(*pNew) );
  if( pNew==0 ){
    assert( pParse->db->mallocFailed );
    pNew = &standin;
  }
  if( pEList==0 ){
    pEList = sqlite3ExprListAppend(pParse, 0,
                                   sqlite3Expr(pParse->db,TK_ASTERISK,0));
  }
  pNew->pEList = pEList;
  pNew->op = TK_SELECT;
  pNew->selFlags = selFlags;
  pNew->iLimit = 0;
  pNew->iOffset = 0;
  pNew->selId = ++pParse->nSelect;
  pNew->addrOpenEphm[0] = -1;
  pNew->addrOpenEphm[1] = -1;
  pNew->nSelectRow = 0;
  if( pSrc==0 ) pSrc = sqlite3DbMallocZero(pParse->db, sizeof(*pSrc));
  pNew->pSrc = pSrc;
  pNew->pWhere = pWhere;
  pNew->pGroupBy = pGroupBy;
  pNew->pHaving = pHaving;
  pNew->pOrderBy = pOrderBy;
  pNew->pPrior = 0;
  pNew->pNext = 0;
  pNew->pLimit = pLimit;
  pNew->pWith = 0;
#ifndef SQLITE_OMIT_WINDOWFUNC
  pNew->pWin = 0;
  pNew->pWinDefn = 0;
#endif
  if( pParse->db->mallocFailed ) {
    clearSelect(pParse->db, pNew, pNew!=&standin);
    pNew = 0;
  }else{
    assert( pNew->pSrc!=0 || pParse->nErr>0 );
  }
  assert( pNew!=&standin );
  return pNew;
}