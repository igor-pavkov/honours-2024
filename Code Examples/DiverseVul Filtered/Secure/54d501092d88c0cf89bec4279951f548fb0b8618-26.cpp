static int zipfileDeflate(
  const u8 *aIn, int nIn,         /* Input */
  u8 **ppOut, int *pnOut,         /* Output */
  char **pzErr                    /* OUT: Error message */
){
  int rc = SQLITE_OK;
  sqlite3_int64 nAlloc;
  z_stream str;
  u8 *aOut;

  memset(&str, 0, sizeof(str));
  str.next_in = (Bytef*)aIn;
  str.avail_in = nIn;
  deflateInit2(&str, 9, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);

  nAlloc = deflateBound(&str, nIn);
  aOut = (u8*)sqlite3_malloc64(nAlloc);
  if( aOut==0 ){
    rc = SQLITE_NOMEM;
  }else{
    int res;
    str.next_out = aOut;
    str.avail_out = nAlloc;
    deflateInit2(&str, 9, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
    res = deflate(&str, Z_FINISH);
    if( res==Z_STREAM_END ){
      *ppOut = aOut;
      *pnOut = (int)str.total_out;
    }else{
      sqlite3_free(aOut);
      *pzErr = sqlite3_mprintf("zipfile: deflate() error");
      rc = SQLITE_ERROR;
    }
    deflateEnd(&str);
  }

  return rc;
}