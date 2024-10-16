static int countOfViewOptimization(Parse *pParse, Select *p){
  Select *pSub, *pPrior;
  Expr *pExpr;
  Expr *pCount;
  sqlite3 *db;
  if( (p->selFlags & SF_Aggregate)==0 ) return 0;   /* This is an aggregate */
  if( p->pEList->nExpr!=1 ) return 0;               /* Single result column */
  if( p->pWhere ) return 0;
  if( p->pGroupBy ) return 0;
  pExpr = p->pEList->a[0].pExpr;
  if( pExpr->op!=TK_AGG_FUNCTION ) return 0;        /* Result is an aggregate */
  if( sqlite3_stricmp(pExpr->u.zToken,"count") ) return 0;  /* Is count() */
  if( pExpr->x.pList!=0 ) return 0;                 /* Must be count(*) */
  if( p->pSrc->nSrc!=1 ) return 0;                  /* One table in FROM  */
  pSub = p->pSrc->a[0].pSelect;
  if( pSub==0 ) return 0;                           /* The FROM is a subquery */
  if( pSub->pPrior==0 ) return 0;                   /* Must be a compound ry */
  do{
    if( pSub->op!=TK_ALL && pSub->pPrior ) return 0;  /* Must be UNION ALL */
    if( pSub->pWhere ) return 0;                      /* No WHERE clause */
    if( pSub->pLimit ) return 0;                      /* No LIMIT clause */
    if( pSub->selFlags & SF_Aggregate ) return 0;     /* Not an aggregate */
    pSub = pSub->pPrior;                              /* Repeat over compound */
  }while( pSub );

  /* If we reach this point then it is OK to perform the transformation */

  db = pParse->db;
  pCount = pExpr;
  pExpr = 0;
  pSub = p->pSrc->a[0].pSelect;
  p->pSrc->a[0].pSelect = 0;
  sqlite3SrcListDelete(db, p->pSrc);
  p->pSrc = sqlite3DbMallocZero(pParse->db, sizeof(*p->pSrc));
  while( pSub ){
    Expr *pTerm;
    pPrior = pSub->pPrior;
    pSub->pPrior = 0;
    pSub->pNext = 0;
    pSub->selFlags |= SF_Aggregate;
    pSub->selFlags &= ~SF_Compound;
    pSub->nSelectRow = 0;
    sqlite3ExprListDelete(db, pSub->pEList);
    pTerm = pPrior ? sqlite3ExprDup(db, pCount, 0) : pCount;
    pSub->pEList = sqlite3ExprListAppend(pParse, 0, pTerm);
    pTerm = sqlite3PExpr(pParse, TK_SELECT, 0, 0);
    sqlite3PExprAddSelect(pParse, pTerm, pSub);
    if( pExpr==0 ){
      pExpr = pTerm;
    }else{
      pExpr = sqlite3PExpr(pParse, TK_PLUS, pTerm, pExpr);
    }
    pSub = pPrior;
  }
  p->pEList->a[0].pExpr = pExpr;
  p->selFlags &= ~SF_Aggregate;

#if SELECTTRACE_ENABLED
  if( sqlite3SelectTrace & 0x400 ){
    SELECTTRACE(0x400,pParse,p,("After count-of-view optimization:\n"));
    sqlite3TreeViewSelect(0, p, 0);
  }
#endif
  return 1;
}