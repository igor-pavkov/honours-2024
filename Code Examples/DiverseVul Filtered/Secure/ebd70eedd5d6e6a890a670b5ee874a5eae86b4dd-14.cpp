static int pragmaVtabBestIndex(sqlite3_vtab *tab, sqlite3_index_info *pIdxInfo){
  PragmaVtab *pTab = (PragmaVtab*)tab;
  const struct sqlite3_index_constraint *pConstraint;
  int i, j;
  int seen[2];

  pIdxInfo->estimatedCost = (double)1;
  if( pTab->nHidden==0 ){ return SQLITE_OK; }
  pConstraint = pIdxInfo->aConstraint;
  seen[0] = 0;
  seen[1] = 0;
  for(i=0; i<pIdxInfo->nConstraint; i++, pConstraint++){
    if( pConstraint->usable==0 ) continue;
    if( pConstraint->op!=SQLITE_INDEX_CONSTRAINT_EQ ) continue;
    if( pConstraint->iColumn < pTab->iHidden ) continue;
    j = pConstraint->iColumn - pTab->iHidden;
    assert( j < 2 );
    seen[j] = i+1;
  }
  if( seen[0]==0 ){
    pIdxInfo->estimatedCost = (double)2147483647;
    pIdxInfo->estimatedRows = 2147483647;
    return SQLITE_OK;
  }
  j = seen[0]-1;
  pIdxInfo->aConstraintUsage[j].argvIndex = 1;
  pIdxInfo->aConstraintUsage[j].omit = 1;
  if( seen[1]==0 ) return SQLITE_OK;
  pIdxInfo->estimatedCost = (double)20;
  pIdxInfo->estimatedRows = 20;
  j = seen[1]-1;
  pIdxInfo->aConstraintUsage[j].argvIndex = 2;
  pIdxInfo->aConstraintUsage[j].omit = 1;
  return SQLITE_OK;
}