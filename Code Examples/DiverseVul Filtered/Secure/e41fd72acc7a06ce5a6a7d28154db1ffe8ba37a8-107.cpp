static void cellUnion(Rtree *pRtree, RtreeCell *p1, RtreeCell *p2){
  int ii = 0;
  if( pRtree->eCoordType==RTREE_COORD_REAL32 ){
    do{
      p1->aCoord[ii].f = MIN(p1->aCoord[ii].f, p2->aCoord[ii].f);
      p1->aCoord[ii+1].f = MAX(p1->aCoord[ii+1].f, p2->aCoord[ii+1].f);
      ii += 2;
    }while( ii<pRtree->nDim2 );
  }else{
    do{
      p1->aCoord[ii].i = MIN(p1->aCoord[ii].i, p2->aCoord[ii].i);
      p1->aCoord[ii+1].i = MAX(p1->aCoord[ii+1].i, p2->aCoord[ii+1].i);
      ii += 2;
    }while( ii<pRtree->nDim2 );
  }
}