void sqlite3WhereMinMaxOptEarlyOut(Vdbe *v, WhereInfo *pWInfo){
  WhereLevel *pInner;
  int i;
  if( !pWInfo->bOrderedInnerLoop ) return;
  if( pWInfo->nOBSat==0 ) return;
  for(i=pWInfo->nLevel-1; i>=0; i--){
    pInner = &pWInfo->a[i];
    if( (pInner->pWLoop->wsFlags & WHERE_COLUMN_IN)!=0 ){
      sqlite3VdbeGoto(v, pInner->addrNxt);
      return;
    }
  }
  sqlite3VdbeGoto(v, pWInfo->iBreak);
}