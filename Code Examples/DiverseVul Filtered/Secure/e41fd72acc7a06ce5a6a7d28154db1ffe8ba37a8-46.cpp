static int rtreeSqlInit(
  Rtree *pRtree, 
  sqlite3 *db, 
  const char *zDb, 
  const char *zPrefix, 
  int isCreate
){
  int rc = SQLITE_OK;

  #define N_STATEMENT 8
  static const char *azSql[N_STATEMENT] = {
    /* Write the xxx_node table */
    "INSERT OR REPLACE INTO '%q'.'%q_node' VALUES(?1, ?2)",
    "DELETE FROM '%q'.'%q_node' WHERE nodeno = ?1",

    /* Read and write the xxx_rowid table */
    "SELECT nodeno FROM '%q'.'%q_rowid' WHERE rowid = ?1",
    "INSERT OR REPLACE INTO '%q'.'%q_rowid' VALUES(?1, ?2)",
    "DELETE FROM '%q'.'%q_rowid' WHERE rowid = ?1",

    /* Read and write the xxx_parent table */
    "SELECT parentnode FROM '%q'.'%q_parent' WHERE nodeno = ?1",
    "INSERT OR REPLACE INTO '%q'.'%q_parent' VALUES(?1, ?2)",
    "DELETE FROM '%q'.'%q_parent' WHERE nodeno = ?1"
  };
  sqlite3_stmt **appStmt[N_STATEMENT];
  int i;
  const int f = SQLITE_PREPARE_PERSISTENT|SQLITE_PREPARE_NO_VTAB;

  pRtree->db = db;

  if( isCreate ){
    char *zCreate;
    sqlite3_str *p = sqlite3_str_new(db);
    int ii;
    sqlite3_str_appendf(p,
       "CREATE TABLE \"%w\".\"%w_rowid\"(rowid INTEGER PRIMARY KEY,nodeno",
       zDb, zPrefix);
    for(ii=0; ii<pRtree->nAux; ii++){
      sqlite3_str_appendf(p,",a%d",ii);
    }
    sqlite3_str_appendf(p,
      ");CREATE TABLE \"%w\".\"%w_node\"(nodeno INTEGER PRIMARY KEY,data);",
      zDb, zPrefix);
    sqlite3_str_appendf(p,
    "CREATE TABLE \"%w\".\"%w_parent\"(nodeno INTEGER PRIMARY KEY,parentnode);",
      zDb, zPrefix);
    sqlite3_str_appendf(p,
       "INSERT INTO \"%w\".\"%w_node\"VALUES(1,zeroblob(%d))",
       zDb, zPrefix, pRtree->iNodeSize);
    zCreate = sqlite3_str_finish(p);
    if( !zCreate ){
      return SQLITE_NOMEM;
    }
    rc = sqlite3_exec(db, zCreate, 0, 0, 0);
    sqlite3_free(zCreate);
    if( rc!=SQLITE_OK ){
      return rc;
    }
  }

  appStmt[0] = &pRtree->pWriteNode;
  appStmt[1] = &pRtree->pDeleteNode;
  appStmt[2] = &pRtree->pReadRowid;
  appStmt[3] = &pRtree->pWriteRowid;
  appStmt[4] = &pRtree->pDeleteRowid;
  appStmt[5] = &pRtree->pReadParent;
  appStmt[6] = &pRtree->pWriteParent;
  appStmt[7] = &pRtree->pDeleteParent;

  rc = rtreeQueryStat1(db, pRtree);
  for(i=0; i<N_STATEMENT && rc==SQLITE_OK; i++){
    char *zSql;
    const char *zFormat;
    if( i!=3 || pRtree->nAux==0 ){
       zFormat = azSql[i];
    }else {
       /* An UPSERT is very slightly slower than REPLACE, but it is needed
       ** if there are auxiliary columns */
       zFormat = "INSERT INTO\"%w\".\"%w_rowid\"(rowid,nodeno)VALUES(?1,?2)"
                  "ON CONFLICT(rowid)DO UPDATE SET nodeno=excluded.nodeno";
    }
    zSql = sqlite3_mprintf(zFormat, zDb, zPrefix);
    if( zSql ){
      rc = sqlite3_prepare_v3(db, zSql, -1, f, appStmt[i], 0); 
    }else{
      rc = SQLITE_NOMEM;
    }
    sqlite3_free(zSql);
  }
  if( pRtree->nAux ){
    pRtree->zReadAuxSql = sqlite3_mprintf(
       "SELECT * FROM \"%w\".\"%w_rowid\" WHERE rowid=?1",
       zDb, zPrefix);
    if( pRtree->zReadAuxSql==0 ){
      rc = SQLITE_NOMEM;
    }else{
      sqlite3_str *p = sqlite3_str_new(db);
      int ii;
      char *zSql;
      sqlite3_str_appendf(p, "UPDATE \"%w\".\"%w_rowid\"SET ", zDb, zPrefix);
      for(ii=0; ii<pRtree->nAux; ii++){
        if( ii ) sqlite3_str_append(p, ",", 1);
        if( ii<pRtree->nAuxNotNull ){
          sqlite3_str_appendf(p,"a%d=coalesce(?%d,a%d)",ii,ii+2,ii);
        }else{
          sqlite3_str_appendf(p,"a%d=?%d",ii,ii+2);
        }
      }
      sqlite3_str_appendf(p, " WHERE rowid=?1");
      zSql = sqlite3_str_finish(p);
      if( zSql==0 ){
        rc = SQLITE_NOMEM;
      }else{
        rc = sqlite3_prepare_v3(db, zSql, -1, f, &pRtree->pWriteAux, 0); 
        sqlite3_free(zSql);
      }
    }
  }

  return rc;
}