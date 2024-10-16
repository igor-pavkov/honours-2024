int sqlite3_vtab_distinct(sqlite3_index_info *pIdxInfo){
  HiddenIndexInfo *pHidden = (HiddenIndexInfo*)&pIdxInfo[1];
  assert( pHidden->eDistinct>=0 && pHidden->eDistinct<=3 );
  return pHidden->eDistinct;
}