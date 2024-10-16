static void zipfileEntryFree(ZipfileEntry *p){
  if( p ){
    sqlite3_free(p->cds.zFile);
    sqlite3_free(p);
  }
}