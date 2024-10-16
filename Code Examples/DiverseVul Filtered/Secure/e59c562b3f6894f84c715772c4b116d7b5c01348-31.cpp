static CollSeq *multiSelectCollSeq(Parse *pParse, Select *p, int iCol){
  CollSeq *pRet;
  if( p->pPrior ){
    pRet = multiSelectCollSeq(pParse, p->pPrior, iCol);
  }else{
    pRet = 0;
  }
  assert( iCol>=0 );
  /* iCol must be less than p->pEList->nExpr.  Otherwise an error would
  ** have been thrown during name resolution and we would not have gotten
  ** this far */
  if( pRet==0 && ALWAYS(iCol<p->pEList->nExpr) ){
    pRet = sqlite3ExprCollSeq(pParse, p->pEList->a[iCol].pExpr);
  }
  return pRet;
}