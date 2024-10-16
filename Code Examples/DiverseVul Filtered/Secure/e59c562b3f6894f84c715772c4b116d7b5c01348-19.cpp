static void selectAddSubqueryTypeInfo(Walker *pWalker, Select *p){
  Parse *pParse;
  int i;
  SrcList *pTabList;
  struct SrcList_item *pFrom;

  assert( p->selFlags & SF_Resolved );
  if( p->selFlags & SF_HasTypeInfo ) return;
  p->selFlags |= SF_HasTypeInfo;
  pParse = pWalker->pParse;
  pTabList = p->pSrc;
  for(i=0, pFrom=pTabList->a; i<pTabList->nSrc; i++, pFrom++){
    Table *pTab = pFrom->pTab;
    assert( pTab!=0 );
    if( (pTab->tabFlags & TF_Ephemeral)!=0 ){
      /* A sub-query in the FROM clause of a SELECT */
      Select *pSel = pFrom->pSelect;
      if( pSel ){
        while( pSel->pPrior ) pSel = pSel->pPrior;
        sqlite3SelectAddColumnTypeAndCollation(pParse, pTab, pSel,
                                               SQLITE_AFF_NONE);
      }
    }
  }
}