static ZipfileEntry *zipfileNewEntry(const char *zPath){
  ZipfileEntry *pNew;
  pNew = sqlite3_malloc(sizeof(ZipfileEntry));
  if( pNew ){
    memset(pNew, 0, sizeof(ZipfileEntry));
    pNew->cds.zFile = sqlite3_mprintf("%s", zPath);
    if( pNew->cds.zFile==0 ){
      sqlite3_free(pNew);
      pNew = 0;
    }
  }
  return pNew;
}