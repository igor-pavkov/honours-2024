static int rtreeOpen(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor){
  int rc = SQLITE_NOMEM;
  Rtree *pRtree = (Rtree *)pVTab;
  RtreeCursor *pCsr;

  pCsr = (RtreeCursor *)sqlite3_malloc64(sizeof(RtreeCursor));
  if( pCsr ){
    memset(pCsr, 0, sizeof(RtreeCursor));
    pCsr->base.pVtab = pVTab;
    rc = SQLITE_OK;
    pRtree->nCursor++;
  }
  *ppCursor = (sqlite3_vtab_cursor *)pCsr;

  return rc;
}