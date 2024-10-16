static int indexMightHelpWithOrderBy(
  WhereLoopBuilder *pBuilder,
  Index *pIndex,
  int iCursor
){
  ExprList *pOB;
  ExprList *aColExpr;
  int ii, jj;

  if( pIndex->bUnordered ) return 0;
  if( (pOB = pBuilder->pWInfo->pOrderBy)==0 ) return 0;
  for(ii=0; ii<pOB->nExpr; ii++){
    Expr *pExpr = sqlite3ExprSkipCollateAndLikely(pOB->a[ii].pExpr);
    if( NEVER(pExpr==0) ) continue;
    if( pExpr->op==TK_COLUMN && pExpr->iTable==iCursor ){
      if( pExpr->iColumn<0 ) return 1;
      for(jj=0; jj<pIndex->nKeyCol; jj++){
        if( pExpr->iColumn==pIndex->aiColumn[jj] ) return 1;
      }
    }else if( (aColExpr = pIndex->aColExpr)!=0 ){
      for(jj=0; jj<pIndex->nKeyCol; jj++){
        if( pIndex->aiColumn[jj]!=XN_EXPR ) continue;
        if( sqlite3ExprCompareSkip(pExpr,aColExpr->a[jj].pExpr,iCursor)==0 ){
          return 1;
        }
      }
    }
  }
  return 0;
}