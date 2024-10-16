static void whereInfoFree(sqlite3 *db, WhereInfo *pWInfo){
  assert( pWInfo!=0 );
  sqlite3WhereClauseClear(&pWInfo->sWC);
  while( pWInfo->pLoops ){
    WhereLoop *p = pWInfo->pLoops;
    pWInfo->pLoops = p->pNextLoop;
    whereLoopDelete(db, p);
  }
  assert( pWInfo->pExprMods==0 );
  while( pWInfo->pMemToFree ){
    WhereMemBlock *pNext = pWInfo->pMemToFree->pNext;
    sqlite3DbFreeNN(db, pWInfo->pMemToFree);
    pWInfo->pMemToFree = pNext;
  }
  sqlite3DbFreeNN(db, pWInfo);
}