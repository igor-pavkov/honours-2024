int sqlite3ExpandSubquery(Parse *pParse, struct SrcList_item *pFrom){
  Select *pSel = pFrom->pSelect;
  Table *pTab;

  assert( pSel );
  pFrom->pTab = pTab = sqlite3DbMallocZero(pParse->db, sizeof(Table));
  if( pTab==0 ) return SQLITE_NOMEM;
  pTab->nTabRef = 1;
  if( pFrom->zAlias ){
    pTab->zName = sqlite3DbStrDup(pParse->db, pFrom->zAlias);
  }else{
    pTab->zName = sqlite3MPrintf(pParse->db, "subquery_%u", pSel->selId);
  }
  while( pSel->pPrior ){ pSel = pSel->pPrior; }
  sqlite3ColumnsFromExprList(pParse, pSel->pEList,&pTab->nCol,&pTab->aCol);
  pTab->iPKey = -1;
  pTab->nRowLogEst = 200; assert( 200==sqlite3LogEst(1048576) );
  pTab->tabFlags |= TF_Ephemeral;

  return pParse->nErr ? SQLITE_ERROR : SQLITE_OK;
}