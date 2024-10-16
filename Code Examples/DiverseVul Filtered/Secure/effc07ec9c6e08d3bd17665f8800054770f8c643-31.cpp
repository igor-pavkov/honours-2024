int sqlite3WhereIsSorted(WhereInfo *pWInfo){
  assert( pWInfo->wctrlFlags & (WHERE_GROUPBY|WHERE_DISTINCTBY) );
  assert( pWInfo->wctrlFlags & WHERE_SORTBYGROUP );
  return pWInfo->sorted;
}