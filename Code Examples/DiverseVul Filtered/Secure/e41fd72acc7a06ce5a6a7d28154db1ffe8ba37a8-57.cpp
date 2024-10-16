static int rtreeConstraintError(Rtree *pRtree, int iCol){
  sqlite3_stmt *pStmt = 0;
  char *zSql; 
  int rc;

  assert( iCol==0 || iCol%2 );
  zSql = sqlite3_mprintf("SELECT * FROM %Q.%Q", pRtree->zDb, pRtree->zName);
  if( zSql ){
    rc = sqlite3_prepare_v2(pRtree->db, zSql, -1, &pStmt, 0);
  }else{
    rc = SQLITE_NOMEM;
  }
  sqlite3_free(zSql);

  if( rc==SQLITE_OK ){
    if( iCol==0 ){
      const char *zCol = sqlite3_column_name(pStmt, 0);
      pRtree->base.zErrMsg = sqlite3_mprintf(
          "UNIQUE constraint failed: %s.%s", pRtree->zName, zCol
      );
    }else{
      const char *zCol1 = sqlite3_column_name(pStmt, iCol);
      const char *zCol2 = sqlite3_column_name(pStmt, iCol+1);
      pRtree->base.zErrMsg = sqlite3_mprintf(
          "rtree constraint failed: %s.(%s<=%s)", pRtree->zName, zCol1, zCol2
      );
    }
  }

  sqlite3_finalize(pStmt);
  return (rc==SQLITE_OK ? SQLITE_CONSTRAINT : rc);
}