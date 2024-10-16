static int zipfileOpen(sqlite3_vtab *p, sqlite3_vtab_cursor **ppCsr){
  ZipfileTab *pTab = (ZipfileTab*)p;
  ZipfileCsr *pCsr;
  pCsr = sqlite3_malloc(sizeof(*pCsr));
  *ppCsr = (sqlite3_vtab_cursor*)pCsr;
  if( pCsr==0 ){
    return SQLITE_NOMEM;
  }
  memset(pCsr, 0, sizeof(*pCsr));
  pCsr->iId = ++pTab->iNextCsrid;
  pCsr->pCsrNext = pTab->pCsrList;
  pTab->pCsrList = pCsr;
  return SQLITE_OK;
}