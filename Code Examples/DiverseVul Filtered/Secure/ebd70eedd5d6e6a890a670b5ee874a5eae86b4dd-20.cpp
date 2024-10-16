static int pragmaVtabFilter(
  sqlite3_vtab_cursor *pVtabCursor, 
  int idxNum, const char *idxStr,
  int argc, sqlite3_value **argv
){
  PragmaVtabCursor *pCsr = (PragmaVtabCursor*)pVtabCursor;
  PragmaVtab *pTab = (PragmaVtab*)(pVtabCursor->pVtab);
  int rc;
  int i, j;
  StrAccum acc;
  char *zSql;

  UNUSED_PARAMETER(idxNum);
  UNUSED_PARAMETER(idxStr);
  pragmaVtabCursorClear(pCsr);
  j = (pTab->pName->mPragFlg & PragFlg_Result1)!=0 ? 0 : 1;
  for(i=0; i<argc; i++, j++){
    const char *zText = (const char*)sqlite3_value_text(argv[i]);
    assert( j<ArraySize(pCsr->azArg) );
    assert( pCsr->azArg[j]==0 );
    if( zText ){
      pCsr->azArg[j] = sqlite3_mprintf("%s", zText);
      if( pCsr->azArg[j]==0 ){
        return SQLITE_NOMEM;
      }
    }
  }
  sqlite3StrAccumInit(&acc, 0, 0, 0, pTab->db->aLimit[SQLITE_LIMIT_SQL_LENGTH]);
  sqlite3_str_appendall(&acc, "PRAGMA ");
  if( pCsr->azArg[1] ){
    sqlite3_str_appendf(&acc, "%Q.", pCsr->azArg[1]);
  }
  sqlite3_str_appendall(&acc, pTab->pName->zName);
  if( pCsr->azArg[0] ){
    sqlite3_str_appendf(&acc, "=%Q", pCsr->azArg[0]);
  }
  zSql = sqlite3StrAccumFinish(&acc);
  if( zSql==0 ) return SQLITE_NOMEM;
  rc = sqlite3_prepare_v2(pTab->db, zSql, -1, &pCsr->pPragma, 0);
  sqlite3_free(zSql);
  if( rc!=SQLITE_OK ){
    pTab->base.zErrMsg = sqlite3_mprintf("%s", sqlite3_errmsg(pTab->db));
    return rc;
  }
  return pragmaVtabNext(pVtabCursor);
}