static Table *isSimpleCount(Select *p, AggInfo *pAggInfo){
  Table *pTab;
  Expr *pExpr;

  assert( !p->pGroupBy );

  if( p->pWhere || p->pEList->nExpr!=1 
   || p->pSrc->nSrc!=1 || p->pSrc->a[0].pSelect
  ){
    return 0;
  }
  pTab = p->pSrc->a[0].pTab;
  pExpr = p->pEList->a[0].pExpr;
  assert( pTab && !pTab->pSelect && pExpr );

  if( IsVirtual(pTab) ) return 0;
  if( pExpr->op!=TK_AGG_FUNCTION ) return 0;
  if( NEVER(pAggInfo->nFunc==0) ) return 0;
  if( (pAggInfo->aFunc[0].pFunc->funcFlags&SQLITE_FUNC_COUNT)==0 ) return 0;
  if( ExprHasProperty(pExpr, EP_Distinct|EP_WinFunc) ) return 0;

  return pTab;
}