static i64 nodeGetRowid(
  Rtree *pRtree,       /* The overall R-Tree */
  RtreeNode *pNode,    /* The node from which to extract the ID */
  int iCell            /* The cell index from which to extract the ID */
){
  assert( iCell<NCELL(pNode) );
  return readInt64(&pNode->zData[4 + pRtree->nBytesPerCell*iCell]);
}