static void traceQueue(RtreeCursor *pCur, const char *zPrefix){
  int ii;
  printf("=== %9s ", zPrefix);
  if( pCur->bPoint ){
    tracePoint(&pCur->sPoint, -1, pCur);
  }
  for(ii=0; ii<pCur->nPoint; ii++){
    if( ii>0 || pCur->bPoint ) printf("              ");
    tracePoint(&pCur->aPoint[ii], ii, pCur);
  }
}