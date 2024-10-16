static void rtreeCheckAppendMsg(RtreeCheck *pCheck, const char *zFmt, ...){
  va_list ap;
  va_start(ap, zFmt);
  if( pCheck->rc==SQLITE_OK && pCheck->nErr<RTREE_CHECK_MAX_ERROR ){
    char *z = sqlite3_vmprintf(zFmt, ap);
    if( z==0 ){
      pCheck->rc = SQLITE_NOMEM;
    }else{
      pCheck->zReport = sqlite3_mprintf("%z%s%z", 
          pCheck->zReport, (pCheck->zReport ? "\n" : ""), z
      );
      if( pCheck->zReport==0 ){
        pCheck->rc = SQLITE_NOMEM;
      }
    }
    pCheck->nErr++;
  }
  va_end(ap);
}