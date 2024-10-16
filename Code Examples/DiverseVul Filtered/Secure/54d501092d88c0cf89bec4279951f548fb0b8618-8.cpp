void zipfileFinal(sqlite3_context *pCtx){
  ZipfileCtx *p;
  ZipfileEOCD eocd;
  sqlite3_int64 nZip;
  u8 *aZip;

  p = (ZipfileCtx*)sqlite3_aggregate_context(pCtx, sizeof(ZipfileCtx));
  if( p==0 ) return;
  if( p->nEntry>0 ){
    memset(&eocd, 0, sizeof(eocd));
    eocd.nEntry = (u16)p->nEntry;
    eocd.nEntryTotal = (u16)p->nEntry;
    eocd.nSize = p->cds.n;
    eocd.iOffset = p->body.n;

    nZip = p->body.n + p->cds.n + ZIPFILE_EOCD_FIXED_SZ;
    aZip = (u8*)sqlite3_malloc64(nZip);
    if( aZip==0 ){
      sqlite3_result_error_nomem(pCtx);
    }else{
      memcpy(aZip, p->body.a, p->body.n);
      memcpy(&aZip[p->body.n], p->cds.a, p->cds.n);
      zipfileSerializeEOCD(&eocd, &aZip[p->body.n + p->cds.n]);
      sqlite3_result_blob(pCtx, aZip, (int)nZip, zipfileFree);
    }
  }

  sqlite3_free(p->body.a);
  sqlite3_free(p->cds.a);
}