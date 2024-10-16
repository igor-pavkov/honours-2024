static int changeTempStorage(Parse *pParse, const char *zStorageType){
  int ts = getTempStore(zStorageType);
  sqlite3 *db = pParse->db;
  if( db->temp_store==ts ) return SQLITE_OK;
  if( invalidateTempStorage( pParse ) != SQLITE_OK ){
    return SQLITE_ERROR;
  }
  db->temp_store = (u8)ts;
  return SQLITE_OK;
}