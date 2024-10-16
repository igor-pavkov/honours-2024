static void nodeGetCoord(
  Rtree *pRtree,               /* The overall R-Tree */
  RtreeNode *pNode,            /* The node from which to extract a coordinate */
  int iCell,                   /* The index of the cell within the node */
  int iCoord,                  /* Which coordinate to extract */
  RtreeCoord *pCoord           /* OUT: Space to write result to */
){
  readCoord(&pNode->zData[12 + pRtree->nBytesPerCell*iCell + 4*iCoord], pCoord);
}