static void freeIndexInfo(sqlite3 *db, sqlite3_index_info *pIdxInfo){
  HiddenIndexInfo *pHidden;
  int i;
  assert( pIdxInfo!=0 );
  pHidden = (HiddenIndexInfo*)&pIdxInfo[1];
  assert( pHidden->pParse!=0 );
  assert( pHidden->pParse->db==db );
  for(i=0; i<pIdxInfo->nConstraint; i++){
    sqlite3ValueFree(pHidden->aRhs[i]); /* IMP: R-14553-25174 */
    pHidden->aRhs[i] = 0;
  }
  sqlite3DbFree(db, pIdxInfo);
}