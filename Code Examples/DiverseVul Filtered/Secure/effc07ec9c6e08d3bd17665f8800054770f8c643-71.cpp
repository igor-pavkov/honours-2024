char sqlite3IndexColumnAffinity(sqlite3 *db, Index *pIdx, int iCol){
  assert( iCol>=0 && iCol<pIdx->nColumn );
  if( !pIdx->zColAff ){
    if( sqlite3IndexAffinityStr(db, pIdx)==0 ) return SQLITE_AFF_BLOB;
  }
  assert( pIdx->zColAff[iCol]!=0 );
  return pIdx->zColAff[iCol];
}