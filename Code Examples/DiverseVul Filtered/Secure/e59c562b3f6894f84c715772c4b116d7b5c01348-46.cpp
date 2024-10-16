Table *sqlite3ResultSetOfSelect(Parse *pParse, Select *pSelect, char aff){
  Table *pTab;
  sqlite3 *db = pParse->db;
  u64 savedFlags;

  savedFlags = db->flags;
  db->flags &= ~(u64)SQLITE_FullColNames;
  db->flags |= SQLITE_ShortColNames;
  sqlite3SelectPrep(pParse, pSelect, 0);
  db->flags = savedFlags;
  if( pParse->nErr ) return 0;
  while( pSelect->pPrior ) pSelect = pSelect->pPrior;
  pTab = sqlite3DbMallocZero(db, sizeof(Table) );
  if( pTab==0 ){
    return 0;
  }
  pTab->nTabRef = 1;
  pTab->zName = 0;
  pTab->nRowLogEst = 200; assert( 200==sqlite3LogEst(1048576) );
  sqlite3ColumnsFromExprList(pParse, pSelect->pEList, &pTab->nCol, &pTab->aCol);
  sqlite3SelectAddColumnTypeAndCollation(pParse, pTab, pSelect, aff);
  pTab->iPKey = -1;
  if( db->mallocFailed ){
    sqlite3DeleteTable(db, pTab);
    return 0;
  }
  return pTab;
}