void *sqlite3WhereRealloc(WhereInfo *pWInfo, void *pOld, u64 nByte){
  void *pNew = sqlite3WhereMalloc(pWInfo, nByte);
  if( pNew && pOld ){
    WhereMemBlock *pOldBlk = (WhereMemBlock*)pOld;
    pOldBlk--;
    assert( pOldBlk->sz<nByte );
    memcpy(pNew, pOld, pOldBlk->sz);
  }
  return pNew;
}