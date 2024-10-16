static int rtreeRowid(sqlite3_vtab_cursor *pVtabCursor, sqlite_int64 *pRowid){
  RtreeCursor *pCsr = (RtreeCursor *)pVtabCursor;
  RtreeSearchPoint *p = rtreeSearchPointFirst(pCsr);
  int rc = SQLITE_OK;
  RtreeNode *pNode = rtreeNodeOfFirstSearchPoint(pCsr, &rc);
  if( rc==SQLITE_OK && p ){
    *pRowid = nodeGetRowid(RTREE_OF_CURSOR(pCsr), pNode, p->iCell);
  }
  return rc;
}