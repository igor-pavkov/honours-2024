static void selectExprDefer(
  Parse *pParse,                  /* Leave any error here */
  SortCtx *pSort,                 /* Sorter context */
  ExprList *pEList,               /* Expressions destined for sorter */
  ExprList **ppExtra              /* Expressions to append to sorter record */
){
  int i;
  int nDefer = 0;
  ExprList *pExtra = 0;
  for(i=0; i<pEList->nExpr; i++){
    struct ExprList_item *pItem = &pEList->a[i];
    if( pItem->u.x.iOrderByCol==0 ){
      Expr *pExpr = pItem->pExpr;
      Table *pTab = pExpr->y.pTab;
      if( pExpr->op==TK_COLUMN && pExpr->iColumn>=0 && pTab && !IsVirtual(pTab)
       && (pTab->aCol[pExpr->iColumn].colFlags & COLFLAG_SORTERREF)
      ){
        int j;
        for(j=0; j<nDefer; j++){
          if( pSort->aDefer[j].iCsr==pExpr->iTable ) break;
        }
        if( j==nDefer ){
          if( nDefer==ArraySize(pSort->aDefer) ){
            continue;
          }else{
            int nKey = 1;
            int k;
            Index *pPk = 0;
            if( !HasRowid(pTab) ){
              pPk = sqlite3PrimaryKeyIndex(pTab);
              nKey = pPk->nKeyCol;
            }
            for(k=0; k<nKey; k++){
              Expr *pNew = sqlite3PExpr(pParse, TK_COLUMN, 0, 0);
              if( pNew ){
                pNew->iTable = pExpr->iTable;
                pNew->y.pTab = pExpr->y.pTab;
                pNew->iColumn = pPk ? pPk->aiColumn[k] : -1;
                pExtra = sqlite3ExprListAppend(pParse, pExtra, pNew);
              }
            }
            pSort->aDefer[nDefer].pTab = pExpr->y.pTab;
            pSort->aDefer[nDefer].iCsr = pExpr->iTable;
            pSort->aDefer[nDefer].nKey = nKey;
            nDefer++;
          }
        }
        pItem->bSorterRef = 1;
      }
    }
  }
  pSort->nDefer = (u8)nDefer;
  *ppExtra = pExtra;
}