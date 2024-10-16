static int rtreeBeginTransaction(sqlite3_vtab *pVtab){
  Rtree *pRtree = (Rtree *)pVtab;
  assert( pRtree->inWrTrans==0 );
  pRtree->inWrTrans++;
  return SQLITE_OK;
}