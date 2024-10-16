static sqlite3_stmt *rtreeCheckPrepare(
  RtreeCheck *pCheck,             /* RtreeCheck object */
  const char *zFmt, ...           /* Format string and trailing args */
){
  va_list ap;
  char *z;
  sqlite3_stmt *pRet = 0;

  va_start(ap, zFmt);
  z = sqlite3_vmprintf(zFmt, ap);

  if( pCheck->rc==SQLITE_OK ){
    if( z==0 ){
      pCheck->rc = SQLITE_NOMEM;
    }else{
      pCheck->rc = sqlite3_prepare_v2(pCheck->db, z, -1, &pRet, 0);
    }
  }

  sqlite3_free(z);
  va_end(ap);
  return pRet;
}