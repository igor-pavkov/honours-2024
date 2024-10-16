static int zipfileClose(sqlite3_vtab_cursor *cur){
  ZipfileCsr *pCsr = (ZipfileCsr*)cur;
  ZipfileTab *pTab = (ZipfileTab*)(pCsr->base.pVtab);
  ZipfileCsr **pp;
  zipfileResetCursor(pCsr);

  /* Remove this cursor from the ZipfileTab.pCsrList list. */
  for(pp=&pTab->pCsrList; *pp!=pCsr; pp=&((*pp)->pCsrNext));
  *pp = pCsr->pCsrNext;

  sqlite3_free(pCsr);
  return SQLITE_OK;
}