static u8 minMaxQuery(sqlite3 *db, Expr *pFunc, ExprList **ppMinMax){
  int eRet = WHERE_ORDERBY_NORMAL;      /* Return value */
  ExprList *pEList = pFunc->x.pList;    /* Arguments to agg function */
  const char *zFunc;                    /* Name of aggregate function pFunc */
  ExprList *pOrderBy;
  u8 sortFlags;

  assert( *ppMinMax==0 );
  assert( pFunc->op==TK_AGG_FUNCTION );
  assert( !IsWindowFunc(pFunc) );
  if( pEList==0 || pEList->nExpr!=1 || ExprHasProperty(pFunc, EP_WinFunc) ){
    return eRet;
  }
  zFunc = pFunc->u.zToken;
  if( sqlite3StrICmp(zFunc, "min")==0 ){
    eRet = WHERE_ORDERBY_MIN;
    sortFlags = KEYINFO_ORDER_BIGNULL;
  }else if( sqlite3StrICmp(zFunc, "max")==0 ){
    eRet = WHERE_ORDERBY_MAX;
    sortFlags = KEYINFO_ORDER_DESC;
  }else{
    return eRet;
  }
  *ppMinMax = pOrderBy = sqlite3ExprListDup(db, pEList, 0);
  assert( pOrderBy!=0 || db->mallocFailed );
  if( pOrderBy ) pOrderBy->a[0].sortFlags = sortFlags;
  return eRet;
}