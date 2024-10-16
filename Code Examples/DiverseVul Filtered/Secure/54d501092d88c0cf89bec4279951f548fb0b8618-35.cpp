static int zipfileBestIndex(
  sqlite3_vtab *tab,
  sqlite3_index_info *pIdxInfo
){
  int i;
  int idx = -1;
  int unusable = 0;

  for(i=0; i<pIdxInfo->nConstraint; i++){
    const struct sqlite3_index_constraint *pCons = &pIdxInfo->aConstraint[i];
    if( pCons->iColumn!=ZIPFILE_F_COLUMN_IDX ) continue;
    if( pCons->usable==0 ){
      unusable = 1;
    }else if( pCons->op==SQLITE_INDEX_CONSTRAINT_EQ ){
      idx = i;
    }
  }
  if( idx>=0 ){
    pIdxInfo->aConstraintUsage[idx].argvIndex = 1;
    pIdxInfo->aConstraintUsage[idx].omit = 1;
    pIdxInfo->estimatedCost = 1000.0;
    pIdxInfo->idxNum = 1;
  }else if( unusable ){
    return SQLITE_CONSTRAINT;
  }
  return SQLITE_OK;
}