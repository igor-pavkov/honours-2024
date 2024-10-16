static struct SrcList_item *isSelfJoinView(
  SrcList *pTabList,           /* Search for self-joins in this FROM clause */
  struct SrcList_item *pThis   /* Search for prior reference to this subquery */
){
  struct SrcList_item *pItem;
  for(pItem = pTabList->a; pItem<pThis; pItem++){
    Select *pS1;
    if( pItem->pSelect==0 ) continue;
    if( pItem->fg.viaCoroutine ) continue;
    if( pItem->zName==0 ) continue;
    assert( pItem->pTab!=0 );
    assert( pThis->pTab!=0 );
    if( pItem->pTab->pSchema!=pThis->pTab->pSchema ) continue;
    if( sqlite3_stricmp(pItem->zName, pThis->zName)!=0 ) continue;
    pS1 = pItem->pSelect;
    if( pItem->pTab->pSchema==0 && pThis->pSelect->selId!=pS1->selId ){
      /* The query flattener left two different CTE tables with identical
      ** names in the same FROM clause. */
      continue;
    }
    if( sqlite3ExprCompare(0, pThis->pSelect->pWhere, pS1->pWhere, -1)
     || sqlite3ExprCompare(0, pThis->pSelect->pHaving, pS1->pHaving, -1) 
    ){
      /* The view was modified by some other optimization such as
      ** pushDownWhereTerms() */
      continue;
    }
    return pItem;
  }
  return 0;
}