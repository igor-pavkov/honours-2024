static void tracePoint(RtreeSearchPoint *p, int idx, RtreeCursor *pCur){
  if( idx<0 ){ printf(" s"); }else{ printf("%2d", idx); }
  printf(" %d.%05lld.%02d %g %d",
    p->iLevel, p->id, p->iCell, p->rScore, p->eWithin
  );
  idx++;
  if( idx<RTREE_CACHE_SZ ){
    printf(" %p\n", pCur->aNode[idx]);
  }else{
    printf("\n");
  }
}