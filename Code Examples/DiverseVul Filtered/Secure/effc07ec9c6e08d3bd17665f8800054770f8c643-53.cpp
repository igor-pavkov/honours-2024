int sqlite3WhereContinueLabel(WhereInfo *pWInfo){
  assert( pWInfo->iContinue!=0 );
  return pWInfo->iContinue;
}