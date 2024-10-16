static void rtreeSearchPointPop(RtreeCursor *p){
  int i, j, k, n;
  i = 1 - p->bPoint;
  assert( i==0 || i==1 );
  if( p->aNode[i] ){
    nodeRelease(RTREE_OF_CURSOR(p), p->aNode[i]);
    p->aNode[i] = 0;
  }
  if( p->bPoint ){
    p->anQueue[p->sPoint.iLevel]--;
    p->bPoint = 0;
  }else if( p->nPoint ){
    p->anQueue[p->aPoint[0].iLevel]--;
    n = --p->nPoint;
    p->aPoint[0] = p->aPoint[n];
    if( n<RTREE_CACHE_SZ-1 ){
      p->aNode[1] = p->aNode[n+1];
      p->aNode[n+1] = 0;
    }
    i = 0;
    while( (j = i*2+1)<n ){
      k = j+1;
      if( k<n && rtreeSearchPointCompare(&p->aPoint[k], &p->aPoint[j])<0 ){
        if( rtreeSearchPointCompare(&p->aPoint[k], &p->aPoint[i])<0 ){
          rtreeSearchPointSwap(p, i, k);
          i = k;
        }else{
          break;
        }
      }else{
        if( rtreeSearchPointCompare(&p->aPoint[j], &p->aPoint[i])<0 ){
          rtreeSearchPointSwap(p, i, j);
          i = j;
        }else{
          break;
        }
      }
    }
  }
}