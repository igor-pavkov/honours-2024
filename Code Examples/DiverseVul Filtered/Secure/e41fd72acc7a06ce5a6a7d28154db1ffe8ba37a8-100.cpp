static RtreeSearchPoint *rtreeSearchPointFirst(RtreeCursor *pCur){
  return pCur->bPoint ? &pCur->sPoint : pCur->nPoint ? pCur->aPoint : 0;
}