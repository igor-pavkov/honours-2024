static int zipfileBufferGrow(ZipfileBuffer *pBuf, int nByte){
  if( pBuf->n+nByte>pBuf->nAlloc ){
    u8 *aNew;
    sqlite3_int64 nNew = pBuf->n ? pBuf->n*2 : 512;
    int nReq = pBuf->n + nByte;

    while( nNew<nReq ) nNew = nNew*2;
    aNew = sqlite3_realloc64(pBuf->a, nNew);
    if( aNew==0 ) return SQLITE_NOMEM;
    pBuf->a = aNew;
    pBuf->nAlloc = (int)nNew;
  }
  return SQLITE_OK;
}