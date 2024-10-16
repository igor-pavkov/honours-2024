static SQLITE_NOINLINE Bitmask whereOmitNoopJoin(
  WhereInfo *pWInfo,
  Bitmask notReady
){
  int i;
  Bitmask tabUsed;

  /* Preconditions checked by the caller */
  assert( pWInfo->nLevel>=2 );
  assert( OptimizationEnabled(pWInfo->pParse->db, SQLITE_OmitNoopJoin) );

  /* These two preconditions checked by the caller combine to guarantee
  ** condition (1) of the header comment */
  assert( pWInfo->pResultSet!=0 );
  assert( 0==(pWInfo->wctrlFlags & WHERE_AGG_DISTINCT) );

  tabUsed = sqlite3WhereExprListUsage(&pWInfo->sMaskSet, pWInfo->pResultSet);
  if( pWInfo->pOrderBy ){
    tabUsed |= sqlite3WhereExprListUsage(&pWInfo->sMaskSet, pWInfo->pOrderBy);
  }
  for(i=pWInfo->nLevel-1; i>=1; i--){
    WhereTerm *pTerm, *pEnd;
    SrcItem *pItem;
    WhereLoop *pLoop;
    pLoop = pWInfo->a[i].pWLoop;
    pItem = &pWInfo->pTabList->a[pLoop->iTab];
    if( (pItem->fg.jointype & (JT_LEFT|JT_RIGHT))!=JT_LEFT ) continue;
    if( (pWInfo->wctrlFlags & WHERE_WANT_DISTINCT)==0
     && (pLoop->wsFlags & WHERE_ONEROW)==0
    ){
      continue;
    }
    if( (tabUsed & pLoop->maskSelf)!=0 ) continue;
    pEnd = pWInfo->sWC.a + pWInfo->sWC.nTerm;
    for(pTerm=pWInfo->sWC.a; pTerm<pEnd; pTerm++){
      if( (pTerm->prereqAll & pLoop->maskSelf)!=0 ){
        if( !ExprHasProperty(pTerm->pExpr, EP_OuterON)
         || pTerm->pExpr->w.iJoin!=pItem->iCursor
        ){
          break;
        }
      }
    }
    if( pTerm<pEnd ) continue;
    WHERETRACE(0xffff, ("-> drop loop %c not used\n", pLoop->cId));
    notReady &= ~pLoop->maskSelf;
    for(pTerm=pWInfo->sWC.a; pTerm<pEnd; pTerm++){
      if( (pTerm->prereqAll & pLoop->maskSelf)!=0 ){
        pTerm->wtFlags |= TERM_CODED;
      }
    }
    if( i!=pWInfo->nLevel-1 ){
      int nByte = (pWInfo->nLevel-1-i) * sizeof(WhereLevel);
      memmove(&pWInfo->a[i], &pWInfo->a[i+1], nByte);
    }
    pWInfo->nLevel--;
    assert( pWInfo->nLevel>0 );
  }
  return notReady;
}