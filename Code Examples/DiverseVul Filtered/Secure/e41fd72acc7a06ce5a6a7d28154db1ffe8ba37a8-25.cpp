static int rtreeEndTransaction(sqlite3_vtab *pVtab){
  Rtree *pRtree = (Rtree *)pVtab;
  pRtree->inWrTrans = 0;
  nodeBlobReset(pRtree);
  return SQLITE_OK;
}