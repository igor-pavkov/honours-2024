static int rtreeSavepoint(sqlite3_vtab *pVtab, int iSavepoint){
  Rtree *pRtree = (Rtree *)pVtab;
  u8 iwt = pRtree->inWrTrans;
  UNUSED_PARAMETER(iSavepoint);
  pRtree->inWrTrans = 0;
  nodeBlobReset(pRtree);
  pRtree->inWrTrans = iwt;
  return SQLITE_OK;
}