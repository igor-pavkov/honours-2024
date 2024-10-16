static int convertCompoundSelectToSubquery(Walker *pWalker, Select *p){
  int i;
  Select *pNew;
  Select *pX;
  sqlite3 *db;
  struct ExprList_item *a;
  SrcList *pNewSrc;
  Parse *pParse;
  Token dummy;

  if( p->pPrior==0 ) return WRC_Continue;
  if( p->pOrderBy==0 ) return WRC_Continue;
  for(pX=p; pX && (pX->op==TK_ALL || pX->op==TK_SELECT); pX=pX->pPrior){}
  if( pX==0 ) return WRC_Continue;
  a = p->pOrderBy->a;
  for(i=p->pOrderBy->nExpr-1; i>=0; i--){
    if( a[i].pExpr->flags & EP_Collate ) break;
  }
  if( i<0 ) return WRC_Continue;

  /* If we reach this point, that means the transformation is required. */

  pParse = pWalker->pParse;
  db = pParse->db;
  pNew = sqlite3DbMallocZero(db, sizeof(*pNew) );
  if( pNew==0 ) return WRC_Abort;
  memset(&dummy, 0, sizeof(dummy));
  pNewSrc = sqlite3SrcListAppendFromTerm(pParse,0,0,0,&dummy,pNew,0,0);
  if( pNewSrc==0 ) return WRC_Abort;
  *pNew = *p;
  p->pSrc = pNewSrc;
  p->pEList = sqlite3ExprListAppend(pParse, 0, sqlite3Expr(db, TK_ASTERISK, 0));
  p->op = TK_SELECT;
  p->pWhere = 0;
  pNew->pGroupBy = 0;
  pNew->pHaving = 0;
  pNew->pOrderBy = 0;
  p->pPrior = 0;
  p->pNext = 0;
  p->pWith = 0;
#ifndef SQLITE_OMIT_WINDOWFUNC
  p->pWinDefn = 0;
#endif
  p->selFlags &= ~SF_Compound;
  assert( (p->selFlags & SF_Converted)==0 );
  p->selFlags |= SF_Converted;
  assert( pNew->pPrior!=0 );
  pNew->pPrior->pNext = pNew;
  pNew->pLimit = 0;
  return WRC_Continue;
}