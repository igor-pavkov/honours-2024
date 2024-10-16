static void substSelect(
  SubstContext *pSubst, /* Description of the substitution */
  Select *p,            /* SELECT statement in which to make substitutions */
  int doPrior           /* Do substitutes on p->pPrior too */
){
  SrcList *pSrc;
  struct SrcList_item *pItem;
  int i;
  if( !p ) return;
  do{
    substExprList(pSubst, p->pEList);
    substExprList(pSubst, p->pGroupBy);
    substExprList(pSubst, p->pOrderBy);
    p->pHaving = substExpr(pSubst, p->pHaving);
    p->pWhere = substExpr(pSubst, p->pWhere);
    pSrc = p->pSrc;
    assert( pSrc!=0 );
    for(i=pSrc->nSrc, pItem=pSrc->a; i>0; i--, pItem++){
      substSelect(pSubst, pItem->pSelect, 1);
      if( pItem->fg.isTabFunc ){
        substExprList(pSubst, pItem->u1.pFuncArg);
      }
    }
  }while( doPrior && (p = p->pPrior)!=0 );
}