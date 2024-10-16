void sqlite3SelectAddColumnTypeAndCollation(
  Parse *pParse,        /* Parsing contexts */
  Table *pTab,          /* Add column type information to this table */
  Select *pSelect,      /* SELECT used to determine types and collations */
  char aff              /* Default affinity for columns */
){
  sqlite3 *db = pParse->db;
  NameContext sNC;
  Column *pCol;
  CollSeq *pColl;
  int i;
  Expr *p;
  struct ExprList_item *a;

  assert( pSelect!=0 );
  assert( (pSelect->selFlags & SF_Resolved)!=0 );
  assert( pTab->nCol==pSelect->pEList->nExpr || db->mallocFailed );
  if( db->mallocFailed ) return;
  memset(&sNC, 0, sizeof(sNC));
  sNC.pSrcList = pSelect->pSrc;
  a = pSelect->pEList->a;
  for(i=0, pCol=pTab->aCol; i<pTab->nCol; i++, pCol++){
    const char *zType;
    int n, m;
    p = a[i].pExpr;
    zType = columnType(&sNC, p, 0, 0, 0);
    /* pCol->szEst = ... // Column size est for SELECT tables never used */
    pCol->affinity = sqlite3ExprAffinity(p);
    if( zType ){
      m = sqlite3Strlen30(zType);
      n = sqlite3Strlen30(pCol->zName);
      pCol->zName = sqlite3DbReallocOrFree(db, pCol->zName, n+m+2);
      if( pCol->zName ){
        memcpy(&pCol->zName[n+1], zType, m+1);
        pCol->colFlags |= COLFLAG_HASTYPE;
      }
    }
    if( pCol->affinity<=SQLITE_AFF_NONE ) pCol->affinity = aff;
    pColl = sqlite3ExprCollSeq(pParse, p);
    if( pColl && pCol->zColl==0 ){
      pCol->zColl = sqlite3DbStrDup(db, pColl->zName);
    }
  }
  pTab->szTabRow = 1; /* Any non-zero value works */
}